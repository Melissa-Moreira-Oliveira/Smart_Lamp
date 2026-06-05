using backend.Models;
using backend.Services;
using Microsoft.AspNetCore.Mvc;

namespace backend.Controllers;

[ApiController]
[Route("api/telemetry")]
public sealed class SensoresController : ControllerBase
{
    private readonly SensorService _sensorService;

    public SensoresController(SensorService sensorService)
    {
        _sensorService = sensorService;
    }

    /// <summary>
    /// Receives telemetry from the ESP32.
    /// </summary>
    [HttpPost]
    [ProducesResponseType(typeof(SensorData), StatusCodes.Status200OK)]
    public ActionResult<SensorData> PostTelemetry([FromBody] TelemetryRequest request)
    {
        // ModelState is automatically validated because of [ApiController].
        var latest = _sensorService.Update(request);
        return Ok(latest);
    }

    /// <summary>
    /// Returns the latest telemetry snapshot (or 204 if none received yet).
    /// </summary>
    [HttpGet("latest")]
    [ProducesResponseType(typeof(SensorData), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status204NoContent)]
    public ActionResult<SensorData> GetLatest()
    {
        var latest = _sensorService.GetLatest();
        if (latest is null) return NoContent();
        return Ok(latest);
    }
}