import 'whatwg-fetch';
import React from 'react';
import { NavLink } from 'react-router-dom';

export class ActionPeripheral extends React.Component {
    render() {
        return (
            <h1>Actionable Peripheral</h1>
        );
    }
}

class Peripheral extends React.Component {
    renderServices() {
        let services = [];

        services = [this.props.input_services.map((service) => {
            return (
                <li>Input&mdash;Service ID: {service.service.id}, Number: {service.service_number}, Name: {service.service.name}</li>
            );
        })];

        return [... services, this.props.output_services.map((service) => {
            return (
                <li>Output&mdash;Service ID: {service.service.id}, Number: {service.service_number}, Name: {service.service.name}</li>
            );
        })];
    }

    render() {
        return (
            <li>
                Name: <NavLink to={`/peripheral/${this.props.id}`}>{this.props.name}</NavLink>
                (ID: {this.props.id},
                 Address: {this.props.address},
                 Queue: {this.props.queue}
                )
                <ul>
                    {this.renderServices()}
                </ul>
            </li>
        );
    }
}

export class ListPeripherals extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            'peripherals': []
        }
    }

    componentDidMount() {
        fetch('/api/v1/peripherals/').then((response) => {
            return response.json();
        }).then((json) => {
            this.setState({
                'peripherals': json.peripheral
            })
        });
    }

    renderPeripherals() {
        return this.state.peripherals.map((peripheral) => {
            return <Peripheral key={peripheral.id}
                               id={peripheral.id}
                               address={peripheral.address}
                               name={peripheral.name}
                               queue={peripheral.queue}
                               input_services={peripheral.input_services}
                               output_services={peripheral.output_services} />
        });
    }

    render() {
        return (
            <ul>
                {this.renderPeripherals()}
            </ul>
        )
    }
}
