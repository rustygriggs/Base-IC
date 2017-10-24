import React from 'react';

import { BrowserRouter, Route, Switch } from 'react-router-dom';
import { ListPeripherals, ActionPeripheral } from './components/peripherals';
import NoMatch from './components/no-match';

// Layouts
import Navigation from './components/navigation';

export default (
    <BrowserRouter>
        <div>
            <Navigation/>
            <div className="container mt-4">
                <Switch>
                    <Route path="/" exact={true} component={ListPeripherals}/>
                    <Route path="/peripheral/:peripheralid" component={ActionPeripheral}/>
                    <Route component={NoMatch}/>
                </Switch>
            </div>
        </div>
    </BrowserRouter>
);
