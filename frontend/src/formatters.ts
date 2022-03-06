export function formatTemperature(value: number) {
  return `${value.toFixed(1)} °C`;
}

export function formatHumidity(value: number) {
  return `${value.toFixed(0)} %`;
}

export function formatTimestamp(value: number) {
  const d = new Date(value * 1000);
  return d.toLocaleTimeString([], {
    timeZone: Intl.DateTimeFormat().resolvedOptions().timeZone,
    hour12: false,
    hour: "2-digit",
    minute: "2-digit",
    second: "2-digit",
  });
}
