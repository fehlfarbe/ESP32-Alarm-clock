import Tabs from '@mui/material/Tabs';
import LinkTab from '@mui/material/Tab';
import Box from '@mui/material/Box';
import DeviceStateControl from './DeviceStateControl';
import React from 'react';
import Tab from '@mui/material/Tab';
import SongsPage from './PlayerPage';
import ConfigPage from './ConfigPage';
import {
    MemoryRouter,
    Route,
    Routes,
    Link,
    matchPath,
    useLocation,
} from 'react-router-dom';

function useRouteMatch(patterns: readonly string[]) {
    const { pathname } = useLocation();

    for (let i = 0; i < patterns.length; i += 1) {
        const pattern = patterns[i];
        const possibleMatch = matchPath(pattern, pathname);
        if (possibleMatch !== null) {
            return possibleMatch;
        }
    }

    return null;
}

interface NavBarProps {
    initialValue: string;
}

const NavBar = (props: NavBarProps) => {
    const { initialValue, ...other } = props
    const [value, setValue] = React.useState<string>(initialValue);
    const routeMatch = useRouteMatch(['/', '/config']);
    const currentTab = routeMatch?.pattern?.path;

    const handleChange = (event: React.SyntheticEvent, newValue: string) => {
        setValue(newValue);
    };

    return (
        <Box sx={{ borderBottom: 1, borderColor: 'divider' }}>
            <Tabs
                value={value}
                onChange={handleChange}
                aria-label="nav tabs example"
                role="navigation"
            >
                <Tab label="Player" value="/" to={"/"} component={Link} />
                <Tab label="Config" value="/config" to={"/config"} component={Link} />
                <div style={{ verticalAlign: 'middle', marginLeft: '10px' }}>
                    <DeviceStateControl />
                </div>
            </Tabs>
        </Box>
    )
}

export default NavBar;