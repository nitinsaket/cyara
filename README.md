# cyara

There are 3 projects under this repository:

- DataProvider
- WorkerService
- Aggregator

DataProvider: A data provider service which accepts new connections -- upon connection establishment a GUID should be sent to the connected party.

WorkerService: A corresponding worker service that is capable of being instantiated multiple times concurrently, which initiates connections to the data provider service. This service should receive the GUID and count the number of non-hexadecimal digits [0-9] in the GUID. The service will then take the result and transmit it to Aggregator.

Aggregator: An aggregation service which receives the resulting count from each worker service, and provides the following metrics: min/max/average/total digits/total responses.

Note: The folder 'Common' holds the code that's common between the projects.
