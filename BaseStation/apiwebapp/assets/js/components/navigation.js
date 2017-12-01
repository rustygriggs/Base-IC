import React from 'react';
import {Link, NavLink} from 'react-router-dom';

export default class Navigation extends React.Component {
  render() {
    return (
      <nav className="navbar navbar-expand-md navbar-dark bg-dark fixed-top">
        <Link className="navbar-brand" to="/">Base-IC</Link>

        <button className="navbar-toggler"
                type="button"
                data-toggle="collapse"
                data-target="#navbarsExampleDefault"
                aria-controls="navbarsExampleDefault"
                aria-expanded="false"
                aria-label="Toggle navigation">
          <span className="navbar-toggler-icon"/>
        </button>

        <div className="collapse navbar-collapse" id="navbarsExampleDefault">
          <ul className="navbar-nav mr-auto">
            <li className="nav-item"><NavLink activeClassName="active" exact={true} className="nav-link" to="/">Peripherals</NavLink></li>
            <li className="nav-item"><NavLink activeClassName="active" exact={true} className="nav-link" to="/recipes">Recipes</NavLink></li>
          </ul>
        </div>
      </nav>
    );
  }
}
