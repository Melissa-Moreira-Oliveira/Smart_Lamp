using backend.Models;
using backend.Services;
using Microsoft.AspNetCore.Mvc;

namespace backend.Controllers;

[ApiController]
[Route("api/device")]
public sealed class DeviceController : ControllerBase
{
    private readonly SensorService _sensorService;
    private readonly LampadaService _lampadaService;

    public DeviceController(SensorService sensorService, LampadaService lampadaService)
    {
        _sensorService = sensorService;
        _lampadaService = lampadaService;
    }

    [HttpGet("status")]
    [ProducesResponseType(typeof(DeviceStatus), StatusCodes.Status200OK)]
    public ActionResult<DeviceStatus> GetStatus()
    {
        return Ok(new DeviceStatus
        {
            LatestTelemetry = _sensorService.GetLatest(),
            Command = _lampadaService.Get()
        });
    }
}