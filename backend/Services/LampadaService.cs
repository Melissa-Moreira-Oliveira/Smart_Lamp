using backend.Models;

namespace backend.Services;

/// <summary>
/// Stores the desired lamp mode/command (in-memory).
/// The ESP32 polls this via GET /api/command.
/// </summary>
public sealed class LampadaService
{
    private readonly object _sync = new();
    private LampState _state = LampState.Default;

    public LampState Get()
    {
        lock (_sync)
        {
            return _state;
        }
    }

    public LampState Set(LampState newState)
    {
        lock (_sync)
        {
            _state = newState;
            return _state;
        }
    }

    public LampState SetAuto()
    {
        lock (_sync)
        {
            _state = _state with { Mode = OperatingMode.Auto };
            return _state;
        }
    }

    public LampState SetManual(bool lampOn)
    {
        lock (_sync)
        {
            _state = _state with { Mode = OperatingMode.Manual, ManualLampOn = lampOn };
            return _state;
        }
    }
}