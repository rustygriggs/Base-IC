import axios from 'axios';
import React from 'react';
import {RecipesRow} from "./recipes-row";

export class RecipesTable extends React.Component {

  constructor(props) {
    super(props);

    this.state = {
      recipes: []
    };
  }

  componentDidMount() {
    axios.get('/api/v1/recipes/').then((response) => {
      this.setState({
        recipes: response.data.recipes
      });
    });
  }

  render() {
    return (
      <table className="table table-bordered">
        <thead className="thead-dark">
        <tr className="bg-secondary text-center">
          <th colSpan="4">Input</th>
          <th colSpan="3">Output</th>
        </tr>
        </thead>
        <thead className="thead-light">
        <tr>
          <th>Peripheral Name</th>
          <th>Service Name</th>
          <th>Service Type</th>
          <th>Input Value</th>

          <th>Peripheral Name</th>
          <th>Service Type</th>
          <th>Output Value</th>
        </tr>
        </thead>
        <tbody>
        {this.state.recipes.map((row, index) => {
          return <RecipesRow key={index} recipe={row}/>
        })}
        </tbody>
      </table>
    );
  }
}