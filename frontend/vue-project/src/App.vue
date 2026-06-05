<script setup lang="ts">
import { ref, onMounted, onUnmounted, computed } from 'vue'

const API = 'http://localhost:5000'

interface SensorData {
  pir: boolean
  ldr: number
  lampOn: boolean
  mode: 'auto' | 'manual'
  uptimeMs: number
  receivedAtUtc: string
}

interface LampState {
  mode: 'auto' | 'manual'
  manualLampOn: boolean
}

interface DeviceStatus {
  latestTelemetry: SensorData | null
  command: LampState
}

const status = ref<DeviceStatus | null>(null)
const error = ref<string | null>(null)
const loading = ref(false)
let pollInterval: ReturnType<typeof setInterval> | null = null

async function fetchStatus() {
  try {
    const res = await fetch(`${API}/api/device/status`)
    if (!res.ok) throw new Error(`HTTP ${res.status}`)
    status.value = await res.json()
    error.value = null
  } catch (e) {
    error.value = 'Cannot reach backend'
  }
}

async function setAuto() {
  loading.value = true
  try {
    await fetch(`${API}/api/command/auto`, { method: 'POST' })
    await fetchStatus()
  } finally {
    loading.value = false
  }
}

async function setManual(lampOn: boolean) {
  loading.value = true
  try {
    await fetch(`${API}/api/command/manual?lampOn=${lampOn}`, { method: 'POST' })
    await fetchStatus()
  } finally {
    loading.value = false
  }
}

const uptime = computed(() => {
  const ms = status.value?.latestTelemetry?.uptimeMs ?? 0
  const s = Math.floor(ms / 1000)
  const m = Math.floor(s / 60)
  const h = Math.floor(m / 60)
  return `${h}h ${m % 60}m ${s % 60}s`
})

const lastSeen = computed(() => {
  const ts = status.value?.latestTelemetry?.receivedAtUtc
  if (!ts) return '—'
  return new Date(ts).toLocaleTimeString()
})


onMounted(() => {
  fetchStatus()
  pollInterval = setInterval(fetchStatus, 2000)
})

onUnmounted(() => {
  if (pollInterval) clearInterval(pollInterval)
})
</script>

<template>
  <div class="app">
    <header>
      <div class="logo">
        <span class="icon">💡</span>
        <span>Controle da Lâmpada Inteligente.</span>
      </div>
    </header>

    <main>
      <div v-if="error" class="error-banner">{{ error }}</div>

      <div class="grid">
        <!-- Sensor readings -->
        <div class="card">
          <div class="card-title">Movimento (PIR)</div>
          <div class="sensor-value" :class="status?.latestTelemetry?.pir ? 'active' : 'inactive'">
            <span class="big-icon">{{ status?.latestTelemetry?.pir ? '🟢' : '❌' }}</span>
            <span class="label">{{ status?.latestTelemetry?.pir ? 'Detectado' : 'Não Detectado' }}</span>
          </div>
        </div>

        <div class="card">
          <div class="card-title">Nível de Luz (LDR)</div>
          <div class="sensor-value">
            <span class="ldr-number">{{ status?.latestTelemetry?.ldr ?? '—' }}</span>
            <span class="ldr-unit">ADC (0–4095)</span>
          </div>
        </div>

        <div class="card">
          <div class="card-title">Lâmpada</div>
          <div class="sensor-value" :class="status?.latestTelemetry?.lampOn ? 'active' : 'inactive'">
            <span class="big-icon">{{ status?.latestTelemetry?.lampOn ? '🟢' : '❌' }}</span>
            <span class="label">{{ status?.latestTelemetry?.lampOn ? 'Ligada' : 'Desligada' }}</span>
          </div>
        </div>

        <div class="card">
          <div class="card-title">Tempo de Funcionamento (decorrido).</div>
          <div class="uptime">
            <span class="uptime-value">{{ status?.latestTelemetry ? uptime : '—' }}</span>
          </div>
        </div>
      </div>

      <!-- Controls -->
      <div class="card controls-card">
        <div class="card-title">Controle</div>
        <span class="label">Altere o modo de operação da lâmpada, o modo automático permite que a lâmpada seja controlada automaticamente com base nas leituras dos sensores. E o modo manual permite que a lâmpada seja controlada manualmente.</span>
        <span class="label">Ativar o modo manual desativa o controle automático, mas mantém a lâmpada ligada ou desligada conforme a última configuração manual, se o modo automático for ativado, o modo manual se desativa.</span>
        <div class="mode-display">
          Modo Atual:
          <span class="mode-badge" :class="status?.command.mode">
            {{ status?.command.mode ?? '—' }}
          </span>
        </div>
        <div class="controls">
          <button
            class="btn btn-secondary"
            :class="{ active: status?.command.mode === 'auto' }"
            :disabled="loading"
            @click="setAuto"
          >
            Automático
          </button>
          <button
            class="btn btn-secondary"
            :class="{ active: status?.command.mode === 'manual' && !status?.command.manualLampOn }"
            :disabled="loading"
            @click="setManual(false)"
          >
            Off (modo manual)
          </button>
          <button
            class="btn btn-primary"
            :class="{ active: status?.command.mode === 'manual' && status?.command.manualLampOn }"
            :disabled="loading"
            @click="setManual(true)"
          >
            On (modo manual)
          </button>
        </div>
      </div>
    </main>
  </div>
</template>

<style scoped>
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

.app {
  min-height: 100vh;
  background: #0f1117;
  color: #e2e8f0;
  font-family: 'Inter', system-ui, sans-serif;
}

header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 1rem 2rem;
  border-bottom: 1px solid #1e2332;
  background: #13161f;
}

.logo {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  font-size: 1.1rem;
  font-weight: 600;
  letter-spacing: 0.02em;
}

.icon {
  font-size: 1.3rem;
}

.status-dot {
  width: 10px;
  height: 10px;
  border-radius: 50%;
}

.status-dot.online {
  background: #22c55e;
  box-shadow: 0 0 6px #22c55e88;
}

.status-dot.offline {
  background: #ef4444;
}

main {
  max-width: 900px;
  margin: 0 auto;
  padding: 2rem 1.5rem;
}

.error-banner {
  background: #3b1515;
  border: 1px solid #7f1d1d;
  color: #fca5a5;
  border-radius: 8px;
  padding: 0.75rem 1rem;
  margin-bottom: 1.5rem;
  font-size: 0.9rem;
}

.grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
  gap: 1rem;
  margin-bottom: 1rem;
}

.card {
  background: #13161f;
  border: 1px solid #1e2332;
  border-radius: 12px;
  padding: 1.25rem;
}

.card-title {
  font-size: 0.75rem;
  font-weight: 500;
  text-transform: uppercase;
  letter-spacing: 0.08em;
  color: #64748b;
  margin-bottom: 1rem;
}

.sensor-value {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 0.4rem;
}

.big-icon {
  font-size: 2.2rem;
}

.label {
  font-size: 0.95rem;
  font-weight: 600;
}

.active .label {
  color: #22c55e;
}

.inactive .label {
  color: #94a3b8;
}

.ldr-number {
  font-size: 1.6rem;
  font-weight: 700;
  color: #a5b4fc;
}

.ldr-unit {
  font-size: 0.72rem;
  color: #64748b;
}

.uptime {
  display: flex;
  flex-direction: column;
  gap: 0.4rem;
}

.uptime-value {
  font-size: 1.1rem;
  font-weight: 600;
  color: #a5b4fc;
}

.last-seen {
  font-size: 0.75rem;
  color: #64748b;
}

.controls-card {
  margin-top: 0;
}

.mode-display {
  font-size: 0.85rem;
  color: #94a3b8;
  margin-bottom: 1rem;
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.mode-badge {
  font-weight: 600;
  padding: 0.1rem 0.5rem;
  border-radius: 4px;
  font-size: 0.8rem;
  text-transform: uppercase;
}

.mode-badge.auto {
  background: #1e3a5f;
  color: #60a5fa;
}

.mode-badge.manual {
  background: #3b2a1a;
  color: #fb923c;
}

.controls {
  display: flex;
  gap: 0.75rem;
  flex-wrap: wrap;
}

.btn {
  padding: 0.6rem 1.2rem;
  border-radius: 8px;
  border: 1px solid transparent;
  font-size: 0.9rem;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.15s;
}

.btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.btn-secondary {
  background: #1e2332;
  color: #94a3b8;
  border-color: #2d3548;
}

.btn-secondary:hover:not(:disabled) {
  background: #2d3548;
  color: #e2e8f0;
}

.btn-secondary.active {
  background: #1e3a5f;
  color: #60a5fa;
  border-color: #3b82f6;
}

.btn-primary {
  background: #1e2332;
  color: #94a3b8;
  border-color: #2d3548;
}

.btn-primary:hover:not(:disabled) {
  background: #2d3548;
  color: #e2e8f0;
}

.btn-primary.active {
  background: #14532d;
  color: #4ade80;
  border-color: #22c55e;
}
</style>
