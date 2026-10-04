# Smart Lamp

Smart Lamp é um sistema de lâmpada inteligente desenvolvido com integração entre hardware (ESP32), backend em ASP.NET Core, frontend em Vue e simulação de ambiente IoT. O projeto combina sensores de presença e luminosidade para controlar a iluminação de forma automática ou manual.

## Visão geral

O sistema foi pensado para demonstrar uma arquitetura realista de IoT:
- ESP32 lê sensores e envia dados para o backend
- backend recebe telemetria e controla o estado da lâmpada
- frontend exibe o status do sistema e permite comandos
- simulação pode ser usada para testar a lógica antes de deploy em hardware real

## Funcionalidades

- Detecção de presença via sensor PIR
- Leitura de luminosidade via sensor LDR
- Modo automático com lógica de acionamento da lâmpada
- Modo manual para ligar/desligar a lâmpada manualmente
- API REST para comunicação com o hardware
- Dashboard web em Vue para acompanhamento do sistema
- Simulação de ambiente para testes e prototipagem

## Arquitetura do projeto

```text
ESP32 / Hardware
   │
   ├─ sensor PIR
   ├─ sensor LDR
   └─ LED / lâmpada
   │
   ▼
Firmware em C++ (Arduino/ESP32)
   │
   ├─ lê sensores
   ├─ consulta comandos do backend
   └─ envia telemetria
   │
   ▼
Backend em ASP.NET Core (C#)
   ├─ Controllers
   ├─ Services
   ├─ Models
   └─ API REST
   │
   ▼
Frontend em Vue
   └─ visualização do estado atual da lâmpada
```

## Estrutura do repositório

```text
Smart_Lamp/
├── arduino_ide/
│   └── main.cpp
├── backend/
│   ├── Controllers/
│   ├── Models/
│   ├── Services/
│   ├── Program.cs
│   ├── appsettings.json
│   ├── backend.csproj
│   └── ...
├── frontend/
│   └── vue-project/
│       ├── src/
│       ├── package.json
│       ├── vite.config.ts
│       └── ...
├── simulation/
│   └── ativ1/
├── Apresentação.pdf
├── Apresentação 1.pptx
├── README.md
└── ...
```

## Backend

A API do projeto está localizada na pasta `backend` e foi desenvolvida em ASP.NET Core.

### Principais endpoints

#### `GET /api/device/status`
Retorna o estado atual do dispositivo, incluindo a última telemetria e o comando atual da lâmpada.

#### `POST /api/telemetry`
Recebe dados do ESP32 com informações de presença, luminosidade, estado atual da lâmpada e uptime.

Exemplo de payload:

```json
{
  "pir": true,
  "ldr": 2100,
  "lampOn": true,
  "mode": "auto",
  "uptimeMs": 125000
}
```

#### `GET /api/telemetry/latest`
Retorna a última telemetria recebida.

#### `GET /api/command`
Retorna o comando atual da lâmpada.

#### `PUT /api/command`
Define o estado do comando atual.

#### `POST /api/command/auto`
Define o modo automático.

#### `POST /api/command/manual?lampOn=true`
Define o modo manual e indica se a lâmpada deve ficar ligada.

## Firmware ESP32

O firmware está em `arduino_ide/main.cpp` e implementa:
- conexão com Wi‑Fi
- leitura de PIR e LDR
- consulta ao backend para saber o comando atual
- aplicação da lógica automática ou manual
- envio de telemetria para o servidor

### Configuração necessária

No arquivo `arduino_ide/main.cpp`, configure as informações do Wi‑Fi e do backend:

```cpp
static const char* WIFI_SSID = "nome-wifi";
static const char* WIFI_PASS = "senha-wifi";
static const char* BASE_URL  = "http://192.168.2.1:5000";
```

Ajuste os valores conforme sua rede local.

## Frontend

O frontend está na pasta `frontend/vue-project` e foi desenvolvido com:
- Vue 3
- TypeScript
- Vite
- Pinia
- Vue Router

### Como rodar o frontend

```bash
cd frontend/vue-project
npm install
npm run dev
```

## Backend em execução

### Requisitos
- .NET SDK compatível com o projeto

### Como rodar

```bash
cd backend
dotnet restore
dotnet run
```

A API pode ser acessada via Swagger:
```text
http://localhost:5000/swagger
```

## Simulação

A pasta `simulation/ativ1` contém o ambiente de simulação usado para testar a lógica do projeto. Ela pode ser usada para validar o comportamento do firmware antes da implementação em bancada real ou em hardware físico.

## Tecnologias utilizadas

- C#
- ASP.NET Core
- C++
- Arduino / ESP32
- Vue 3
- TypeScript
- Vite
- PlatformIO / Wokwi

## Observações

- O projeto foi estruturado para demonstrar um fluxo completo de IoT com hardware, API e interface web.
- O backend atualmente mantém os dados em memória, o que facilita desenvolvimento e testes locais.
- Em produção, seria recomendável adicionar banco de dados, autenticação, logs e controle de sessões.

