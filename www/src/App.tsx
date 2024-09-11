import './App.css';
import { Route, Routes } from 'react-router-dom';
import SongsPage from './components/PlayerPage';
import ConfigPage from './components/ConfigPage';
import NavBar from './components/NavBar';
import WebsocketProvider from './components/WebsocketProvider';

function App() {
  return (
    <WebsocketProvider>
      <div className="App">
        <header>
          <div>
            <NavBar initialValue="/" />
          </div>
        </header>
        <div className="container">
          <Routes>
            <Route path='/' Component={SongsPage}/>
            <Route path='/config' Component={ConfigPage} />
          </Routes>
        </div>
      </div>
    </WebsocketProvider>
  );
}

export default App;
