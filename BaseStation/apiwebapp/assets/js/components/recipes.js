import React from 'react';
import {RecipesTable} from "./recipes-table";
import axios from 'axios';

export class Recipes extends React.Component {

  constructor(props) {
    super(props);

    this.state = {
      peripherals: []
    };
  }

  componentDidMount() {
    axios.get('/api/v1/peripherals/').then(response => {
      this.setState({
        peripherals: response.data.peripherals
      });
    });
  }

  render() {

    let newBtn = "";
    if(this.state.peripherals.length > 0) {
      newBtn = <button className="btn btn-link">New</button>;
    }

    return (
      <div className="recipesContainer">
        <h1>Recipes {newBtn}</h1>
        <RecipesTable/>
      </div>
    );
  }
}