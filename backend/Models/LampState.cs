namespace backend.Models;

public sealed record LampState(
    OperatingMode Mode,
    bool ManualLampOn)
{
    public static LampState Default { get; } = new(OperatingMode.Auto, false);
}