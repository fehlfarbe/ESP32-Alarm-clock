import Tabs from '@mui/material/Tabs';
import LinkTab from '@mui/material/Tab';
import Box from '@mui/material/Box';
import DeviceStateControl from './DeviceStateControl';
import React from 'react';

interface NavBarProps {
    initialValue: number;
}

const NavBar = (props: NavBarProps) => {
    const { initialValue, ...other } = props

    const [value, setValue] = React.useState<number>(initialValue);

    return (
        <Box sx={{ borderBottom: 1, borderColor: 'divider' }}>
            <Tabs
                value={value}
                // value={handleChange}
                aria-label="nav tabs example"
                role="navigation"
            >
                <LinkTab label="Player" href="/" />
                <LinkTab label="Config" href="/config" />
                <div style={{ verticalAlign: 'middle', marginLeft: '10px' }}>
                    <DeviceStateControl />
                </div>
            </Tabs>
        </Box>
    )
}

export default NavBar;