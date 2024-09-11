import React, { useContext, useEffect, useState } from 'react';
import { WebsocketContext } from './WebsocketProvider';
import SignalWifi0BarIcon from '@mui/icons-material/SignalWifi0Bar';
import SignalWifi1BarIcon from '@mui/icons-material/SignalWifi1Bar';
import SignalWifi2BarIcon from '@mui/icons-material/SignalWifi2Bar';
import SignalWifi3BarIcon from '@mui/icons-material/SignalWifi3Bar';
import SignalWifi4BarIcon from '@mui/icons-material/SignalWifi4Bar';
import Tooltip from '@mui/material/Tooltip';

const WiFiControl = () => {
    const [ready, val, send] = useContext(WebsocketContext);
    const [rssi, setRssi] = useState<number>(0);

    useEffect(() => {
        if (val) {
            setRssi(val?.wifi.rssi);
        }
    }, [val]);

    let wifi;
    if (rssi == 0) {
        wifi = <SignalWifi0BarIcon  fontSize='large'/>
    } else if (rssi > -50) {
        wifi = <SignalWifi4BarIcon  fontSize='large'/>
    } else if (rssi > -60) {
        wifi = <SignalWifi3BarIcon  fontSize='large'/>
    } else if (rssi > -70) {
        wifi = <SignalWifi2BarIcon  fontSize='large'/>
    } else if (rssi > -80) {
        wifi = <SignalWifi1BarIcon  fontSize='large'/>
    } else {
        wifi = <SignalWifi0BarIcon  fontSize='large'/>
    }

    return (
        <Tooltip title={rssi + "dB"}>
            {wifi}
        </Tooltip>
    )
};

export default WiFiControl;