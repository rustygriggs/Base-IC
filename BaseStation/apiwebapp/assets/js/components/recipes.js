import 'whatwg-fetch';
import React from 'react';
import {RecipesTable} from "./recipes-table";

export class Recipes extends React.Component {

  componentDidMount() {
    fetch('/api/v1/recipes/').then((response) => {
      return response.json();
    }).then((json) => {
      console.log(json);
    });
  }

  render() {
    return (
      <div className="recipesContainer">
        <h1>Recipes</h1>
        <RecipesTable/>
      </div>
    );
  }
}