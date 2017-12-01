import React from 'react';

export class EditText extends React.Component {

  constructor(props) {
    super(props);

    this.state = {
      editing: false,
      value: props.value
    };

    this.beginEditing = this.beginEditing.bind(this);
    this.updateValue = this.updateValue.bind(this);
    this.saveValue = this.saveValue.bind(this);
  }

  beginEditing() {
    this.setState({
      editing: true
    });
  }

  updateValue(event) {
    this.setState({
      value: event.target.value,
    });
  }

  saveValue() {
    console.log(this.props);
    this.props.handle(this.props.id, this.state.value);

    this.setState({
      editing: false,
    });
  }

  render() {
    if (this.state.editing) {
      return <input value={this.state.value}
                    ref={input => input && input.focus()}
                    onChange={this.updateValue}
                    onBlur={this.saveValue}/>
    }

    return <button className="btn btn-link" onClick={this.beginEditing}>{this.state.value}</button>
  }
}