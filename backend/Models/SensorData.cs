namespace backend.Models;

public sealed class SensorData
{
    public bool Pir { get; init; }
    public int Ldr { get; init; }
    public bool LampOn { get; init; }
    public OperatingMode Mode { get; init; }
    public uint UptimeMs { get; init; }
    public DateTime ReceivedAtUtc { get; init; }
}