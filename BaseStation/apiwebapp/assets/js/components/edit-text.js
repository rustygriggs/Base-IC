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
    let value = this.state.value;

    if (this.state.value == null) {
      value = "";
    }

    this.props.handle(this.props.id, value);

    this.setState({
      editing: false,
    });
  }

  render() {
    if (this.state.editing) {
      return <input value={this.state.value || ''}
                    ref={input => input && input.focus()}
                    onChange={this.updateValue}
                    onBlur={this.saveValue}/>
    }

    return <a href="javascript:;" onClick={this.beginEditing}>{this.state.value || 'NONE'}</a>;
  }
}