#include <iostream>
#include <chrono>

// Function to generate the lifetime for a service ticket
std::chrono::seconds generateServiceTicketLifetime(int durationInSeconds) {
    return std::chrono::seconds(durationInSeconds);
}

int main() {
    int durationInSeconds = 3600; // Example: 1 hour (3600 seconds)
    std::chrono::seconds serviceTicketLifetime = generateServiceTicketLifetime(durationInSeconds);

    // Output the generated lifetime for the service ticket
    std::cout << "Generated service ticket lifetime: " << serviceTicketLifetime.count() << " seconds" << std::endl;

    return 0;
}