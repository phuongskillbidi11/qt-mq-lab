param(
    [Parameter(Mandatory = $true)]
    [string]$BrokerHost,

    [Parameter(Mandatory = $true)]
    [string]$User
)

$ErrorActionPreference = "Stop"

if ($BrokerHost -notin @("localhost", "127.0.0.1")) {
    Write-Error "Refusing to apply definitions to non-local broker '$BrokerHost'."
    exit 1
}

if ([string]::IsNullOrEmpty($env:RABBITMQ_PASSWORD)) {
    Write-Error "RABBITMQ_PASSWORD is not set."
    exit 1
}

$definitionsPath = Join-Path (Split-Path $PSScriptRoot -Parent) "definitions.json"
$definitions = Get-Content -LiteralPath $definitionsPath -Raw | ConvertFrom-Json
$credentials = "${User}:$env:RABBITMQ_PASSWORD"
$token = [Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($credentials))
$headers = @{ Authorization = "Basic $token" }
$baseUri = "http://${BrokerHost}:15672/api"

function Invoke-ManagementApi {
    param(
        [string]$Method,
        [string]$Uri,
        [object]$Body
    )

    $parameters = @{
        Method = $Method
        Uri = $Uri
        Headers = $headers
    }
    if ($null -ne $Body) {
        $parameters.ContentType = "application/json"
        $parameters.Body = $Body | ConvertTo-Json -Depth 10 -Compress
    }
    Invoke-RestMethod @parameters
}

$exchangeName = [Uri]::EscapeDataString([string]$definitions.exchange.name)
$queueName = [Uri]::EscapeDataString([string]$definitions.queue.name)

Write-Output "Creating exchange: $($definitions.exchange.name) ($($definitions.exchange.type), durable=$($definitions.exchange.durable))"
Invoke-ManagementApi -Method Put -Uri "$baseUri/exchanges/%2F/$exchangeName" -Body @{
    type = [string]$definitions.exchange.type
    durable = [bool]$definitions.exchange.durable
    auto_delete = $false
    internal = $false
    arguments = @{}
}

Write-Output "Creating queue: $($definitions.queue.name) (durable=$($definitions.queue.durable))"
Invoke-ManagementApi -Method Put -Uri "$baseUri/queues/%2F/$queueName" -Body @{
    durable = [bool]$definitions.queue.durable
    auto_delete = $false
    arguments = @{}
}

foreach ($routingKey in $definitions.bindings) {
    Write-Output "Creating binding: $($definitions.exchange.name) --$routingKey--> $($definitions.queue.name)"
    Invoke-ManagementApi -Method Post -Uri "$baseUri/bindings/%2F/e/$exchangeName/q/$queueName" -Body @{
        routing_key = [string]$routingKey
        arguments = @{}
    }
}

$exchange = Invoke-ManagementApi -Method Get -Uri "$baseUri/exchanges/%2F/$exchangeName" -Body $null
$queue = Invoke-ManagementApi -Method Get -Uri "$baseUri/queues/%2F/$queueName" -Body $null
$bindings = Invoke-ManagementApi -Method Get -Uri "$baseUri/bindings/%2F/e/$exchangeName/q/$queueName" -Body $null

Write-Output "Exchange exists: $($exchange.name) ($($exchange.type), durable=$($exchange.durable))"
Write-Output "Queue exists: $($queue.name) (durable=$($queue.durable))"
foreach ($binding in $bindings) {
    Write-Output "Binding exists: $($binding.source) --$($binding.routing_key)--> $($binding.destination)"
}
