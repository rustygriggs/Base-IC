import axios from 'axios';
import React from 'react';

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
        recipes: response.recipes
      });
    });
  }

  render() {
    return (
      <table>

      </table>
    );
  }
}