using backend.Models;

namespace backend.Services;

/// <summary>
/// Stores the latest telemetry received from the ESP32 (in-memory).
/// </summary>
public sealed class SensorService
{
    private readonly object _sync = new();
    private SensorData? _latest;

    public SensorData? GetLatest()
    {
        lock (_sync)
        {
            return _latest;
        }
    }

    public SensorData Update(TelemetryRequest request)
    {
        var snapshot = new SensorData
        {
            Pir = request.Pir,
            Ldr = request.Ldr,
            LampOn = request.LampOn,
            Mode = request.Mode,
            UptimeMs = request.UptimeMs,
            ReceivedAtUtc = DateTime.UtcNow
        };

        lock (_sync)
        {
            _latest = snapshot;
            return snapshot;
        }
    }
}