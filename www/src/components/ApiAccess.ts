import { url } from "inspector";

const { REACT_APP_DEVICE_ENDPOINT } = process.env;

let deviceEndpoint = "";
if(REACT_APP_DEVICE_ENDPOINT !== undefined) {
    deviceEndpoint = REACT_APP_DEVICE_ENDPOINT
}

const getApiUrl = (url: string) => {
    if(deviceEndpoint) {
        return "http://" + deviceEndpoint + url;
    }
    return url;
}

export const URL_WS = deviceEndpoint ? "ws://" + deviceEndpoint + "/ws"  : "/ws"
export const URL_API_SONGS = getApiUrl("/api/songs")
export const URL_API_SONGS_UPDATE = getApiUrl("/api/songs/update")
export const URL_API_CONFIG = getApiUrl("/api/config")
export const URL_API_PLAYBACK = getApiUrl("/api/playback")
