# CanBus_Nucleo_F446RE

## Descrizione del Progetto

Questa repository contiene il codice per la gestione del protocollo CAN Bus, la lettura dei sensori tramite ADC con DMA e la comunicazione seriale per una scheda STM32 Nucleo-F446RE. È utilizzata principalmente come ambiente di test per velocizzare lo sviluppo di funzionalità che verranno successivamente implementate su una scheda Nucleo-F756ZG.

## Caratteristiche Principali

- **ADC con DMA**: Lettura continua da più sensori in modo efficiente e senza sovraccarico della CPU.
- **Protocollo CAN Bus**: Invio e ricezione di messaggi tramite CAN Bus, ideale per applicazioni automotive o di automazione industriale.
- **Comunicazione seriale**: Debug e log tramite interfaccia seriale, con possibilità di monitorare il funzionamento del sistema in tempo reale.

## Utilizzo

### Prerequisiti

- [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
- Scheda Nucleo-F446RE
- Almeno due transceiver CAN, come [MCP2515](https://blog.naver.com/eziya76/221188525127) o [SN65HVD230](https://youtu.be/KHNRftBa1Vc?si=2C673-Au-6wTgh2l)
- Almeno due potenziometri

### Configurazione Hardware

Di seguito il pinout della Nucleo-F446RE utilizzato per questo progetto:

[F446RE Pinout](https://os.mbed.com/platforms/ST-Nucleo-F446RE/)

### Istruzioni di Compilazione

1. Clona la repository:
   ```bash
   git clone https://github.com/tuo-username/CanBus_Nucleo_F446RE.git
2. Apri il progetto con STM32CubeIDE.
3. Compila e carica il codice sulla Nucleo-F446RE.
4. Configura la tua rete CAN Bus e collega la scheda.
5. Monitora la comunicazione seriale per il debug e la verifica del funzionamento.

## Risorse e Tutorial Utili

- **MCP2515**:  
   [Guida all'uso dell'MCP2515](https://blog.naver.com/eziya76/221188525127)  
   [Video Tutorial](https://www.youtube.com/watch?v=sd6FKkfzU2I)

- **SN65HVD230**:  
   [Video Tutorial 1](https://youtu.be/KHNRftBa1Vc?si=2C673-Au-6wTgh2l)  
   [Video Tutorial 2](https://www.youtube.com/watch?v=-lcrrRrKdFg)

- **Potentiometro & MCP2551**:  
   [Video Tutorial](https://www.micropeta.com/video115)

- **ADC**:  
   [Video Tutorial](https://www.youtube.com/watch?v=zipjCtiHYr8)  
   [Approfondimento](https://deepbluembedded.com/stm32-adc-multi-channel-scan-continuous-mode-dma-poll-examples/#stm32-adc-multichannel-scan-continuous-mode-dma-example)

