// swift-tools-version: 5.10
import PackageDescription

let package = Package(
    name: "Algorithm",
    products: [
        .library(name: "Algorithm", targets: ["Algorithm"]),
    ],
    targets: [
        .target(name: "Algorithm", dependencies: []),
        .testTarget(name: "AlgorithmTests", dependencies: ["Algorithm"]),
    ]
)
