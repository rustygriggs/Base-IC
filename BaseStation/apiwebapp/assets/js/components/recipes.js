import React from 'react';
import {RecipesTable} from "./recipes-table";
import axios from 'axios';

export class Recipes extends React.Component {

  constructor(props) {
    super(props);

    this.state = {
      peripherals: [],
      addingNewRow: false
    };

    this.toggleAddingNewRow = this.toggleAddingNewRow.bind(this);
    this.finishedAddingRow = this.finishedAddingRow.bind(this);
  }

  componentDidMount() {
    axios.get('/api/v1/peripherals/').then(response => {
      this.setState({
        peripherals: response.data.peripherals
      });
    });
  }

  toggleAddingNewRow() {
    this.setState({
      addingNewRow: !this.state.addingNewRow,
    });
  }

  finishedAddingRow() {
    this.setState({
      addingNewRow: false,
    });
  }

  render() {

    let newBtn = "";
    if(this.state.peripherals.length > 0) {
      newBtn = <button className="btn btn-link" onClick={this.toggleAddingNewRow}>New</button>;
    }

    return (
      <div className="recipesContainer">
        <h1>Recipes {newBtn}</h1>
        <RecipesTable addingNewRow={this.state.addingNewRow} finishedAddingRow={this.finishedAddingRow}/>
      </div>
    );
  }
}