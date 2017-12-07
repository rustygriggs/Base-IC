import React from 'react';
import {EditText} from "./edit-text";
import axios from 'axios';

export class RecipesRow extends React.Component {

  constructor(props) {
    super(props);

    console.log(props);

    this.updateServiceName = this.updateServiceName.bind(this);
    this.updateInputValue = this.updateInputValue.bind(this);
    this.updateOutputValue = this.updateOutputValue.bind(this);
  }

  updateServiceName(id, newName) {
    axios.post('/api/v1/peripherals/services/' + id, {
      service_name: newName
    }).catch(() => {
      alert('There was an error. Please refresh and try again.');
    });
  }

  updateInputValue(id, newInputValue) {
    axios.post('/api/v1/recipes/' + id, {
      input_value: newInputValue
    }).catch(() => {
      alert('There was an error. Please refresh and try again.');
    });
  }

  updateOutputValue(id, newOutputValue) {
    axios.post('/api/v1/recipes/' + id, {
      output_value: newOutputValue
    }).catch(() => {
      alert('There was an error. Please refresh and try again.');
    });
  }

  render() {
    let inputService = "N/A";
    if (this.props.recipe.input_service.service.id !== 2) {
      inputService = <EditText id={this.props.recipe.id} value={this.props.recipe.input_value} handle={this.updateInputValue}/>
    }

    let outputService = "N/A";
    if (this.props.recipe.output_service.service.id !== 2) {
      outputService = <EditText id={this.props.recipe.id} value={this.props.recipe.output_value} handle={this.updateOutputValue}/>
    }

    return (
      <tr>
        <td>
          {this.props.recipe.input_service.peripheral.name}
        </td>
        <td>
          #{this.props.recipe.input_service.service_number} <EditText id={this.props.recipe.input_service.id} value={this.props.recipe.input_service.service_name} handle={this.updateServiceName}/>
        </td>
        <td>
          {this.props.recipe.input_service.service.name}
        </td>
        <td>
          {inputService}
        </td>
        <td>
          {this.props.recipe.output_service.peripheral.name}
        </td>
        <td>
	  {this.props.recipe.output_service.service.name} (#{this.props.recipe.output_service.service_number})
        </td>
        <td>
          {outputService}
        </td>
        <td>
          <a href="javascript:;" onClick={() => this.props.deleteRecipe(this.props.recipe.id)}>Delete</a>
        </td>
      </tr>
    );
  }
}
