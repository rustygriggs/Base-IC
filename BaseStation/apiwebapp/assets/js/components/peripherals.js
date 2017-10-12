import 'whatwg-fetch';
import React from 'react';

class Peripheral extends React.Component {
    renderServices() {
        let services = [];

        services = [... services, this.props.input_services.map((service) => {
            return <li>Service ID: {service.service.id}, Number: #{service.service_number}, Name: {service.service.name} - Input</li>
        })];

        return [... services, this.props.output_services.map((service) => {
            return <li>Service ID: {service.service.id}, Number: #{service.service_number}, Name: {service.service.name} - Output</li>
        })]
    }

    render() {
        return <ul>
            <li>
                ID: {this.props.id}
                Address: {this.props.address}
                Name: {this.props.name}
                Queue: {this.props.queue}
                <ul>
                    {this.renderServices()}
                </ul>
            </li>
        </ul>
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
        fetch('http://raspberrypi.local/api/v1/peripherals/').then((response) => {
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
            <div>
                {this.renderPeripherals()}
            </div>
        )
    }
}

