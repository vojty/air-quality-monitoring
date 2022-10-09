import { useEffect, useState } from "react";
import useWebSocket, { ReadyState } from "react-use-websocket";
import { Station } from "./components/Station";
import { Box } from "./components/Box";
import { Status } from "./components/Status";
import { formatCo2, formatHumidity, formatTemperature } from "./formatters";
import { Battery } from "./components/Battery";

type SensorData = {
  temperature: number;
  humidity: number;
  messageId: number;
  receivedAt: number; // in seconds
  interval: number; // in seconds
  battery: number; // in percents
};

enum MessageType {
  STATION = "STATION",
  MASTER = "MASTER",
}

type StationMessage = SensorData & {
  type: MessageType.STATION;
  stationId: number;
};

type MasterMessage = {
  type: MessageType.MASTER;
  timestamp: number; // in seconds
  co2: number; // ppm
};

type Message = StationMessage | MasterMessage;

type StationsData = {
  [stationId: number]: SensorData;
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
  const [stationsData, setStationsData] = useState<StationsData>({});
  const [masterData, setMasterData] = useState<MasterMessage>();

  useEffect(() => {
    const message: Message | null = lastJsonMessage;
    if (!message) {
      return;
    }

    if (message.type === MessageType.STATION) {
      const { stationId, ...sensorData } = message;
      setStationsData((prevState) => ({
        ...prevState,
        [stationId]: sensorData,
      }));
    }

    if (message.type === MessageType.MASTER) {
      setMasterData(message);
    }
  }, [lastJsonMessage]);

  return (
    <div style={{ padding: "0.5rem", margin: "0 auto", maxWidth: 400 }}>
      {Object.entries(stationsData).map(([stationId, data]) => (
        <Station
          key={stationId}
          header={
            <div
              style={{
                display: "flex",
                alignItems: "center",
                margin: "0 0.5rem",
              }}
            >
              <div>Station #{stationId}</div>
              <div style={{ marginLeft: "auto" }}>
                <Battery percentage={data.battery} />
              </div>
            </div>
          }
          // @ts-ignore back compatibility
          lastUpdate={data.receivedAt || data.timestamp}
          interval={data.interval}
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
        </Station>
      ))}

      {masterData && (
        <Station
          header={"Main station"}
          lastUpdate={masterData.timestamp}
          interval={0}
        >
          <Box
            icon="💨"
            title={
              <>
                CO<sup>2</sup>
              </>
            }
            text={formatCo2(masterData.co2)}
          />
        </Station>
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
