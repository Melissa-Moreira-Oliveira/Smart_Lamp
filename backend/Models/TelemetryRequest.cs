using System.ComponentModel.DataAnnotations;

namespace backend.Models;

public sealed class TelemetryRequest
{
    [Required]
    public bool Pir { get; init; }

    [Required]
    public int Ldr { get; init; }

    [Required]
    public bool LampOn { get; init; }
    public OperatingMode Mode { get; init; } = OperatingMode.Auto;

    [Required]
    public uint UptimeMs { get; init; }
}

