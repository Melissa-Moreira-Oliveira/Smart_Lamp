using backend.Models;
using backend.Services;
using Microsoft.AspNetCore.Mvc;

namespace backend.Controllers;

[ApiController]
[Route("api/command")]
public sealed class LampadaController : ControllerBase
{
    private readonly LampadaService _lampadaService;

    public LampadaController(LampadaService lampadaService)
    {
        _lampadaService = lampadaService;
    }

    /// <summary>
    /// ESP32 polls this endpoint to get the current desired mode/command.
    /// </summary>
    [HttpGet]
    [ProducesResponseType(typeof(LampState), StatusCodes.Status200OK)]
    public ActionResult<LampState> GetCommand()
    {
        return Ok(_lampadaService.Get());
    }

    /// <summary>
    /// Sets the current desired command.
    /// Use this from Swagger while you don't have a frontend.
    /// </summary>
    [HttpPut]
    [ProducesResponseType(typeof(LampState), StatusCodes.Status200OK)]
    public ActionResult<LampState> SetCommand([FromBody] LampState state)
    {
        var updated = _lampadaService.Set(state);
        return Ok(updated);
    }

    [HttpPost("auto")]
    [ProducesResponseType(typeof(LampState), StatusCodes.Status200OK)]
    public ActionResult<LampState> SetAuto()
    {
        return Ok(_lampadaService.SetAuto());
    }

    [HttpPost("manual")]
    [ProducesResponseType(typeof(LampState), StatusCodes.Status200OK)]
    public ActionResult<LampState> SetManual([FromQuery] bool lampOn = false)
    {
        return Ok(_lampadaService.SetManual(lampOn));
    }
}