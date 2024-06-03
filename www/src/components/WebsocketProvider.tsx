import React, { createContext, FunctionComponent, ReactNode, useContext, useEffect, useMemo, useRef, useState } from 'react';
import ReconnectingWebSocket, { MessageEvent } from 'reconnectingwebsocket';
import { URL_WS } from './ApiAccess';

export interface Notification {
  general: {
    hostname: string,
    time: number,
    free_heap: number
  },
  player: {
    playing: boolean,
    paused: boolean,
    time: number,
    title: string,
    volume: number
  },
  wifi: {
    ssid: string,
    rssi: number
  }
}

export const WebsocketContext = createContext<[boolean, Notification | null, any]>([false, null, () => { }]);


const WebsocketProvider: FunctionComponent<{ children: ReactNode }> = ({ children }: { children: ReactNode }) => {
  const [isReady, setIsReady] = useState<boolean>(false)
  const [val, setVal] = useState<Notification | null>(null)

  const ws = useRef<ReconnectingWebSocket | null>(null);

  const sendJson = (data: object) => {
    ws.current?.send(JSON.stringify(data))
  }


  const createWebSocket = () => {
    const socket = new ReconnectingWebSocket(URL_WS, undefined, { maxReconnectInterval: 10000 })

    socket.onopen = () => setIsReady(true)
    socket.onclose = () => {
      setIsReady(false)
    }
    socket.onmessage = (event: MessageEvent) => setVal(JSON.parse(event.data))
    socket.onerror = (event: Event) => {
      console.log("WS Error...", event);
      setIsReady(false);
    }

    ws.current = socket
  }

  useEffect(() => {
    createWebSocket();

    return () => {
      console.log('Websocket...close??');
      ws.current?.close();
    }
  }, [])

  return (
    <WebsocketContext.Provider value={[isReady, val, sendJson]}>
      {children}
    </WebsocketContext.Provider>
  )
}

export default WebsocketProvider;