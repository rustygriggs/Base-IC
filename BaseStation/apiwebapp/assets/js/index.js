const React = require('react');
const ReactDOM = require('react-dom');

const Hello = React.createClass({
    render: function() {
        return (
            <h1>Hello, React!</h1>
        )
    }
});

ReactDOM.render(<Hello />, document.getElementById('app'));
