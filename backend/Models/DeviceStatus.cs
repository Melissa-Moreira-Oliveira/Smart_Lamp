namespace backend.Models;
public sealed class DeviceStatus
{
    public SensorData? LatestTelemetry { get; init; }
    public LampState Command { get; init; } = LampState.Default;
}