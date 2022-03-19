import { useEffect, useState } from "react";
import useWebSocket, { ReadyState } from "react-use-websocket";
import { Board } from "./components/Board";
import { Box } from "./components/Box";
import { Status } from "./components/Status";
import { formatCo2, formatHumidity, formatTemperature } from "./formatters";

type SensorData = {
  temperature: number;
  humidity: number;
  messageId: number;
  timestamp: number; // in seconds
};

enum MessageType {
  BOARD = "BOARD",
  MASTER = "MASTER",
}

type BoardMessage = SensorData & {
  type: MessageType.BOARD;
  boardId: number;
};

type MasterMessage = {
  type: MessageType.MASTER;
  timestamp: number; // in seconds
  co2: number; // ppm
};

type Message = BoardMessage | MasterMessage;

type BoardsData = {
  [boardId: number]: SensorData;
};

// https://tailwindcss.com/docs/customizing-colors
function getStatusProps(readyState: ReadyState) {
  switch (readyState) {
    case ReadyState.CONNECTING:
      return { color: "#fdba74", animate: true };
    case ReadyState.UNINSTANTIATED:
    case ReadyState.CLOSING:
    case ReadyState.CLOSED:
      return { color: "#ef4444", animate: false };
    case ReadyState.OPEN:
      return { color: "#4ade80", animate: false };
  }
}

const WS_HOSTNAME =
  import.meta.env.VITE_WS_HOSTNAME || window.location.hostname;

export function App() {
  const { lastJsonMessage, readyState } = useWebSocket(
    `ws://${WS_HOSTNAME}/ws`,
    {
      shouldReconnect: () => true,
      retryOnError: true,
      reconnectInterval: 5_000,
    }
  );
  const [boardsData, setBoardsData] = useState<BoardsData>({});
  const [masterData, setMasterData] = useState<MasterMessage>();

  useEffect(() => {
    const message: Message | null = lastJsonMessage;
    if (!message) {
      return;
    }

    if (message.type === MessageType.BOARD) {
      const { boardId, ...sensorData } = message;
      setBoardsData((prevState) => ({
        ...prevState,
        [boardId]: sensorData,
      }));
    }

    if (message.type === MessageType.MASTER) {
      setMasterData(message);
    }
  }, [lastJsonMessage]);

  return (
    <div style={{ padding: "0.5rem", margin: "0 auto", maxWidth: 400 }}>
      {Object.entries(boardsData).map(([boardId, data]) => (
        <Board
          key={boardId}
          title={<>Board #{boardId}</>}
          lastUpdate={data.timestamp}
        >
          <Box
            icon="🌡️"
            title="Temperature"
            text={formatTemperature(data.temperature)}
          />
          <Box
            icon="💧"
            title="Humidity"
            text={formatHumidity(data.humidity)}
          />
        </Board>
      ))}

      {masterData && (
        <Board title={"Main board"} lastUpdate={masterData.timestamp}>
          <Box
            icon="💨"
            title={
              <>
                CO<sup>2</sup>
              </>
            }
            text={formatCo2(masterData.co2)}
          />
        </Board>
      )}

      <div
        style={{
          display: "flex",
          alignItems: "center",
          justifyContent: "center",
          gap: "0.5rem",
          margin: "0.5rem 0",
        }}
      >
        Connection <Status size={10} {...getStatusProps(readyState)} />
      </div>
    </div>
  );
}
