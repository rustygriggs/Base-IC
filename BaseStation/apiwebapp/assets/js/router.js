import React from 'react';

import {Router, Route, IndexRoute} from 'react-router';

import {history} from 'react-router/lib/HashHistory';

import {ListPeripherals} from './components/peripherals';

// Layouts
import {MainLayout} from './components/main-layout';

export default (
    <Router history={history}>
        <Route component={MainLayout}>
            <Route path="/" component={ListPeripherals}/>
        </Route>
    </Router>
);