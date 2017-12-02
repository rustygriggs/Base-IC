import axios from 'axios';
import React from 'react';
import {RecipesRow} from "./recipes-row";
import {RecipesNewRow} from "./recipes-new-row";

export class RecipesTable extends React.Component {

  constructor(props) {
    super(props);

    this.state = {
      recipes: [],
    };

    this.refreshRecipes = this.refreshRecipes.bind(this);
  }

  componentDidMount() {
    this.refreshRecipes();
  }

  refreshRecipes() {
    axios.get('/api/v1/recipes/').then((response) => {
      this.setState({
        recipes: response.data.recipes
      });
    });
  }

  deleteRecipe(id) {
    let that = this;
    axios.delete('/api/v1/recipes/' + id).then(response => {
      that.refreshRecipes();
    });
  }

  render() {
    return (
      <table className="table table-bordered">
        <thead className="thead-dark">
        <tr className="bg-secondary text-center">
          <th colSpan="4">Input</th>
          <th colSpan="4">Output</th>
        </tr>
        </thead>
        <thead className="thead-light">
        <tr>
          <th>Peripheral Name</th>
          <th>Service Name (#)</th>
          <th>Service Type</th>
          <th>Input Value</th>

          <th>Peripheral Name</th>
          <th>Service Type</th>
          <th>Output Value</th>
          <th>Actions</th>
        </tr>
        </thead>
        <tbody>
        {this.state.recipes.map((row, index) => {
          return <RecipesRow key={index} recipe={row} refreshRecipes={this.refreshRecipes} deleteRecipe={this.deleteRecipe}/>
        })}
        </tbody>
        <tfoot>
        <RecipesNewRow peripherals={this.state.peripherals} addingNewRow={this.props.addingNewRow} refreshRecipes={this.refreshRecipes} finishedAddingRow={this.props.finishedAddingRow}/>
        </tfoot>
      </table>
    );
  }
}