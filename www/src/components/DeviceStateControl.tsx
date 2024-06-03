import React, { useContext, useEffect, useState } from 'react';
import { WebsocketContext } from './WebsocketProvider';
import WiFiControl from './WiFiControl';
import LeakRemoveIcon from '@mui/icons-material/LeakRemove';
import TimeControl from './TimeControl';


const DeviceStateControl = () => {
    const [ready, val, send] = useContext(WebsocketContext);
    const [rssi, setRssi] = useState<number>(0);
    const [connected, setConnected] = useState<boolean>(false);

    useEffect(() => {
        if (val) {
            setRssi(val?.wifi.rssi);
        }
        setConnected(ready);
    }, [ready, val]);

    return (
        <div style={{verticalAlign: 'middle'}}>
            {!connected ?
                <LeakRemoveIcon fontSize='large' />
                :
                <>
                    <WiFiControl />
                    <TimeControl />
                </>
            }
        </div>
    )
};

export default DeviceStateControl;