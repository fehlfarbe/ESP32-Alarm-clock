import React, { useContext, useEffect, useState } from 'react';
import { WebsocketContext } from './WebsocketProvider';


const TimeControl = () => {
    const [ready, val, send] = useContext(WebsocketContext);
    const [time, setTime] = useState<Date>(new Date());
    const [connected, setConnected] = useState<boolean>(false);

    useEffect(() => {
        if (val) {
            setTime(new Date(val?.general.time * 1000));
        }
    }, [val]);

    return (
        <span>
            {
                time.getHours().toString().padStart(2, "0") + ":"
                + time.getMinutes().toString().padStart(2, "0") + ":"
                + time.getSeconds().toString().padStart(2, "0")
            }
        </span>
    )
};

export default TimeControl;