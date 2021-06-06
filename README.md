# cyara

There are 4 projects under this repository:

- DataProvider
- WorkerService
- Aggregator
- UnitTest

DataProvider: A data provider service which accepts new connections -- upon connection establishment a GUID should be sent to the connected party.

WorkerService: A corresponding worker service that is capable of being instantiated multiple times concurrently, which initiates connections to the data provider service. This service should receive the GUID and count the number of non-hexadecimal digits [0-9] in the GUID. The service will then take the result and transmit it to Aggregator.

Aggregator: An aggregation service which receives the resulting count from each worker service, and provides the following metrics: min/max/average/total digits/total responses.

UnitTest: Project that holds UT test cases for the whole code.

Note: The folder 'Common' holds the code that's common between the projects.


# Environment Details

The projects were developed in C++ on Windows 10 Home Edition using Visual Studio 2017.


# Running The System

Note: Build the projects DataProvider, Aggregator and WorkerService in Visual Studio. The components can be executed from Visual Studio environment itself or from command line - command line being easier option to test.

Run servers DataProvider and Aggregator (in any order) before running WorkerService.

Usage:

1. Data Provider: Simply execute 'DataProvider.exe'

2. Aggregator Service: Simply execute 'Aggregator.exe'

3. Worker Service:
- WorkerService.exe
- WorkerService.exe <Data-Provider's IP Address> <Aggregator's IP Address>

Note: Simply executing 'WorkerService.exe' will assume both servers to be on localhost.

Note: Default behaviour is same as 'WorkerService.exe' if invalid params are provided at the command line.


# Running Unit Tests

Simply build and execute the project UnitTest for all test cases to execute.

Unit Tests were written using Catch2 Unit Testing Framework.
