import React from 'react';
import axios from 'axios';

export class RecipesNewRow extends React.Component {

  constructor(props) {
    super(props);

    this.state = {
      peripherals: [],
      inputPeripheralServiceId: "",
      inputValue: "",
      outputPeripheralServiceId: "",
      outputValue: "",
    };

    this.handleInputPeripheralChange = this.handleInputPeripheralChange.bind(this);
    this.handleOutputPeripheralChange = this.handleOutputPeripheralChange.bind(this);
    this.handleInputValueChange = this.handleInputValueChange.bind(this);
    this.handleOutputValueChange = this.handleOutputValueChange.bind(this);
    this.handleSave = this.handleSave.bind(this)
  }

  /********************
   * Lifecycle Methods
   ********************/

  componentDidMount() {
    axios.get('/api/v1/peripherals/').then((response) => {
      this.setState({
        peripherals: response.data.peripherals
      });
    })
  }

  /********************
   * Handlers
   ********************/

  handleInputPeripheralChange(event) {
    this.setState({
      inputPeripheralServiceId: event.target.value
    });
  }

  handleInputValueChange(event) {
    this.setState({
      inputValue: event.target.value
    });
  }

  handleOutputPeripheralChange(event) {
    this.setState({
      outputPeripheralServiceId: event.target.value
    });
  }

  handleOutputValueChange(event) {
    this.setState({
      outputValue: event.target.value
    });
  }

  handleSave(event) {
    event.preventDefault();

    let valid = true;

    if (!this.state.inputPeripheralServiceId) {
      alert("An input peripheral service is required");
      valid = false;
    }

    if (valid && !this.state.inputValue) {
      alert("An input value is required");
      valid = false;
    }

    if (valid && !this.state.outputPeripheralServiceId) {
      alert("An output peripheral service is required");
      valid = false;
    }

    if (valid && !this.state.outputValue) {
      alert("An output value is required");
      valid = false;
    }

    if (valid) {
      axios.post('/api/v1/recipes/', {
        input_peripheral_service_id: this.state.inputPeripheralServiceId,
        input_value: this.state.inputValue,
        output_peripheral_service_id: this.state.outputPeripheralServiceId,
        output_value: this.state.outputValue,
      }).then(response => {
        this.props.refreshRecipes();
        this.setState({
          inputPeripheralServiceId: "",
          inputValue: "",
          outputPeripheralServiceId: "",
          outputValue: "",
        });
        this.props.finishedAddingRow();
      }).catch(response => {
        alert('There was an error saving your new recipe. Please refresh the page and try again.');
        console.log(response);
      });
    }
  }

  /********************
   * Render Methods
   ********************/

  renderServicesArray(services, placeholderText) {
    return [
      <option key={-1}>{placeholderText}</option>,
      ...services.map((option, index) => {
        return <option value={option.serviceId} key={index}>{option.peripheralName} - {option.serviceType} (#{option.serviceNumber}) {option.serviceName && `- ${option.serviceName}`}</option>
      })];
  }

  getServiceArray(peripheral, services) {
    return services.map((service) => {
      return {
        peripheralId: peripheral.id,
        peripheralName: peripheral.name,
        serviceId: service.id,
        serviceNumber: service.service_number,
        serviceType: service.service.name,
        serviceName: service.service_name
      };
    });
  }

  renderInputPeripheralOptions() {
    let inputServices = this.state.peripherals.reduce((accumulator, peripheral) => {
      return [...accumulator, ...this.getServiceArray(peripheral, peripheral.input_services)];
    }, []);

    return this.renderServicesArray(inputServices, 'Select an input service');
  }

  renderOutputPeripheralOptions() {
    let outputServices = this.state.peripherals.reduce((accumulator, peripheral) => {
      return [...accumulator, ...this.getServiceArray(peripheral, peripheral.output_services)];
    }, []);

    return this.renderServicesArray(outputServices, 'Select an output service');
  }

  render() {
    if (!this.props.addingNewRow) {
      return null;
    }

    return <tr>
      <td colSpan="3">
        <select value={this.state.inputPeripheralServiceId} onChange={this.handleInputPeripheralChange}>
          {this.renderInputPeripheralOptions()}
        </select>
      </td>
      <td><input placeholder="Input Value" value={this.state.inputValue} onChange={this.handleInputValueChange}/></td>
      <td colSpan="2">
        <select value={this.state.outputPeripheralServiceId} onChange={this.handleOutputPeripheralChange}>
          {this.renderOutputPeripheralOptions()}
        </select>
      </td>
      <td><input placeholder="Output Value" value={this.state.outputValue} onChange={this.handleOutputValueChange}/></td>
      <td>
        <button onClick={this.handleSave} className="btn btn-sm btn-success">Save</button>
      </td>
    </tr>;
  }
}
