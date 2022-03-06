import { ReactNode, useEffect, useState } from "react";
import useWebSocket, { ReadyState } from "react-use-websocket";
import {
  formatHumidity,
  formatTemperature,
  formatTimestamp,
} from "./formatters";

type SensorData = {
  temperature: number;
  humidity: number;
  messageId: number;
  timestamp: number; // in seconds
};

type Message = SensorData & {
  boardId: number;
};

type State = {
  [boardId: number]: SensorData;
};

function Box({
  icon,
  title,
  text,
}: {
  icon: ReactNode;
  title: ReactNode;
  text: ReactNode;
}) {
  return (
    <div
      style={{
        alignItems: "center",
        display: "flex",
        padding: "1.5rem",
        margin: "1rem",
        background: "#ffedd5",
        borderRadius: 10,
      }}
    >
      <div>
        <div style={{ fontSize: "1.85rem", margin: "0 0.5rem 0.25rem 0" }}>
          {icon}
        </div>
        <div style={{ fontSize: "0.9rem" }}>{title}</div>
      </div>
      <div style={{ fontSize: "3.45rem", marginLeft: "auto" }}>{text}</div>
    </div>
  );
}

// https://tailwindcss.com/docs/customizing-colors
function getReadyStateColor(readyState: ReadyState) {
  switch (readyState) {
    case ReadyState.CONNECTING:
      return "#fdba74";
    case ReadyState.UNINSTANTIATED:
    case ReadyState.CLOSING:
    case ReadyState.CLOSED:
      return "#ef4444";
    case ReadyState.OPEN:
      return "#4ade80";
  }
}

export function App() {
  const { lastJsonMessage, readyState } = useWebSocket("ws://192.168.1.20/ws");
  const [state, setState] = useState<State>({});

  useEffect(() => {
    const message: Message | null = lastJsonMessage;
    if (!message) {
      return;
    }

    const { boardId, ...sensorData } = message;
    setState((prevState) => ({
      ...prevState,
      [boardId]: sensorData,
    }));
  }, [lastJsonMessage]);

  return (
    <>
      {Object.entries(state).map(([boardId, data]) => (
        <div
          key={boardId}
          style={{
            margin: "0 auto",
            maxWidth: 400,
          }}
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

          <small
            style={{
              justifyContent: "center",
              display: "flex",
              alignItems: "center",
            }}
          >
            <span
              style={{
                margin: "0 0.5rem",
                height: 10,
                width: 10,
                background: getReadyStateColor(readyState),
                borderRadius: "50%",
              }}
            />
            Last update @ {formatTimestamp(data.timestamp)}
          </small>
        </div>
      ))}
    </>
  );
}
