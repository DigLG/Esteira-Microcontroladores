import { useState } from 'react'
import playPauseIcon from './assets/playpause.png'
import stopIcon from './assets/stop.png'
import './App.css'

// Array de indicadores com título e valor, no momento zero, será alterado para herdar do backend
const indicadores = [
  { titulo: 'Máquinas direcionadas para uso', valor: 0 },
  { titulo: 'Máquinas direcionadas para conserto', valor: 0 },
  { titulo: 'Máquinas direcionadas para descarte', valor: 0 },
  { titulo: 'Máquinas avaliadas na última hora', valor: 0 },
]

function App() {
  const destinoAtual = 'Aguardando classificação' // será usado no futuro para mostrar destino da caixa atual
  const [status, setStatus] = useState('stopped') // estado atual do processo
  const [mensagemErro, setMensagemErro] = useState('') // texto de erro
  const [carregando, setCarregando] = useState(false) // indica se está aguardando resposta do backend

  async function handlePlayPause() { // async permite o uso de await dentro da função
    setMensagemErro('')
    setCarregando(true)

    try {
      const response = await fetch('http://localhost:8000/playpause')
      if (!response.ok) { // Verifica se a resposta do backend foi bem-sucedida
        throw new Error(`Backend respondeu com status ${response.status}`) 
      }
      const data = await response.json()
      setStatus(data.status)
    } catch (error) {
      console.error('Erro ao chamar Play/Pause:', error)
      setMensagemErro('Falha ao executar Play/Pause')
    } finally {
      setCarregando(false)
    }
  }

  async function handleStop() {
    setMensagemErro('')
    setCarregando(true)

    try {
      const response = await fetch('http://localhost:8000/stop')
      if (!response.ok) { // Verifica se a resposta do backend foi bem-sucedida
        throw new Error(`Backend respondeu com status ${response.status}`)
      }
      setStatus('stopped')
    } catch (error) {
      console.error('Erro ao chamar Stop:', error)
      setMensagemErro('Falha ao interromper o processo')
    } finally {
      setCarregando(false)
    }
  }

  // Status do processo
  const rotulosStatus = {
    playing: 'Em execução',
    paused: 'Pausado',
    stopped: 'Parado',
  }

  return (
    <main className="dashboard">
      <header className="dashboard-header">
        <div>
          <p className="eyebrow">PAINEL DE MONITORAMENTO</p>
          <h1>Direcionamento das máquinas</h1>
        </div>
        {/* Mostra o status atual */}
        <span className={`status-badge status-badge--${status}`}>
          {rotulosStatus[status] || 'Status desconhecido'}
        </span>
      </header>

      <section className="destination-card" aria-live="polite">
        <h2>Máquina direcionada para</h2>
        <p>{destinoAtual}</p>
      </section>

      <section className="metrics-grid" aria-label="Resumo de máquinas">
        {/* Percorre o array de indicadores */}
        {indicadores.map((indicador)  => (
          <article className="metric-card" key={indicador.titulo}>
            <h2>{indicador.titulo}</h2>
            <p>{indicador.valor}</p>
          </article>
        ))}
      </section>

      <section className="controls" aria-label="Controles do processo">
        <button
          className="control-button control-button--play"
          type="button"
          onClick={handlePlayPause}
          disabled={carregando}
        >
          <img className="control-icon" src={playPauseIcon} alt="" />
          {/* Se status for 'playing', mostra 'Pausar', se for 'paused', mostra 'Continuar' e caso contrário, mostra 'Play' */}
          {status === 'playing' ? 'Pausar' : status === 'paused' ? 'Continuar' : 'Play'}
        </button>
        <button
          className="control-button control-button--stop"
          type="button"
          onClick={handleStop}
          disabled={carregando}
        >
          <img className="control-icon" src={stopIcon} alt="" />
          Stop
        </button>
      </section>

      {mensagemErro && (
        <p className="error-message" role="alert">
          {mensagemErro}
        </p>
      )}
    </main>
  )
}

export default App
