# PHP Endpoint Tester
This is some work that I had done to automate and perform health checks on server endpoints. In this example we have a game that is running with a server authoritative pattern where:
* The client must call various endpoints any time an action is taken
* The server then processes those actions 
* The server then reports back to client what the state is 

Because of this, we need to regularly make sure the endpoints on the server are functioning as we expect.
This can be seen as a form of _black box unit testing_, where the tests are written from the outside and only knowing what we expect to see. We aren't paying attention to whatever sorcery the server is doing behind the scenes.

## Explanation
The code here would be run by a Jenkins job that would execute at a set interval. The job script will load a Couchbase document with configuration values that dictate:
* How the validation is supposed to execute
* Which endpoints are to be hit
* Which mock player data to use 

The code will then perform a net request to the endpoints and check the return data from the server to make sure it lines up with the expected results (that part has been removed from this sample).