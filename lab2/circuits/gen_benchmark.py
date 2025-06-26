#!/usr/bin/python3
import pathlib
import random
import argparse
import math
import sys

PINS_PER_TILE = 4


class Block:
    def __init__(self, idx, goal_net_connections):
        self.idx = idx
        self.goal_net_connections = goal_net_connections
        self.nets = []


class BlockIO(Block):
    def __init__(self, idx, goal_net_connections, x, y):
        super().__init__(idx, goal_net_connections)
        self.x = x
        self.y = y


class Pin:
    def __init__(self, parent_tile, idx):
        self.tile = parent_tile
        self.idx = idx
        self.used = False


class Net:
    def __init__(self, idx):
        self.idx = idx
        self.blocks = []


def distance(pin1, pin2):
    return abs(pin1.tile.x - pin2.tile.x) + abs(pin1.tile.y - pin2.tile.y)


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("output_file", help="Output file path", type=pathlib.Path)
    parser.add_argument("size", help="Side size of FPGA")
    parser.add_argument("chip_density", help="Percentage of blocks to use", type=float)
    parser.add_argument(
        "nets_per_block_min", help="Min number of nets per block", type=float
    )
    parser.add_argument(
        "nets_per_block_max", help="Max number of nets per block", type=float
    )
    parser.add_argument(
        "blocks_per_net_avg", help="Avg number of blocks per net", type=float
    )
    parser.add_argument("seed", help="Random seed", type=int, default=0)

    args = parser.parse_args()

    random.seed(args.seed)

    size = int(args.size)
    num_tiles = size**2

    print("-" * 80)
    print("Creating design for " + str(size) + "x" + str(size) + " FPGA")
    print("-" * 80)

    nets = []

    block_idx = 0

    # First figure out I/O blocks
    blocks = []
    io_positons = (
        [(x, 0) for x in range(1, size - 1)]
        + [(x, size - 1) for x in range(1, size - 1)]
        + [(0, y) for y in range(1, size - 1)]
        + [(size - 1, y) for y in range(1, size - 1)]
    )
    num_io_blocks = int(len(io_positons) * args.chip_density)
    print("Will use", num_io_blocks, "IO blocks")
    random.shuffle(io_positons)
    for i in range(num_io_blocks):
        blocks.append(BlockIO(block_idx, 1, io_positons[i][0], io_positons[i][1]))
        block_idx += 1

    # Choose number of logic blocks
    num_blocks = int((size - 2) ** 2 * args.chip_density)
    print(f"Will use {num_blocks} logic blocks")

    # Choose number of nets
    nets_per_block_avg = (args.nets_per_block_min + args.nets_per_block_max) / 2
    total_net_to_block_connections = nets_per_block_avg * num_blocks + num_io_blocks
    total_nets = int(total_net_to_block_connections / args.blocks_per_net_avg)
    # print("Will use", nets_per_block_avg, "nets per block on average")
    print("Will use", total_nets, "nets")

    nets = [Net(i) for i in range(total_nets)]

    # Create blocks
    for i in range(num_blocks):
        blocks.append(
            Block(
                block_idx,
                int(random.uniform(args.nets_per_block_min, args.nets_per_block_max)),
            )
        )
        block_idx += 1

    # Make sure each net connects to at least 2 blocks
    for net in nets:
        block_pair = random.sample(
            [
                b
                for b in blocks
                if len(b.nets) < b.goal_net_connections and not isinstance(b, BlockIO)
            ],
            2,
        )
        block_pair[0].nets.append(net)
        net.blocks.append(block_pair[0])
        block_pair[1].nets.append(net)
        net.blocks.append(block_pair[1])

    # Connect nets to blocks
    for block in blocks:
        # If this is an IO block, only connect to nets that haven't already been connected to an IO block
        if isinstance(block, BlockIO):
            nets_possible = [
                n
                for n in nets
                if len([b for b in n.blocks if isinstance(b, BlockIO)]) == 0
            ]
        else:
            nets_possible = [n for n in nets if n not in block.nets]

        nets_for_this_block = random.sample(
            nets_possible, block.goal_net_connections - len(block.nets)
        )

        block.nets.extend(nets_for_this_block)
        for net in nets_for_this_block:
            net.blocks.append(block)

    # Report number of net connections
    print(f"Min blocks per net: {min([len(n.blocks) for n in nets])}")
    print(f"Max blocks per net: {max([len(n.blocks) for n in nets])}")
    print(f"Avg blocks per net: {sum([len(n.blocks) for n in nets]) / len(nets)}")

    # Make sure each net only connects to at most one IO block
    for net in nets:
        assert len([b for b in net.blocks if isinstance(b, BlockIO)]) <= 1

    # Write out to file
    with open(args.output_file, "w") as fd:
        fd.write(f"{size} {size}\n")

        # Output fixed blocks
        for block in [b for b in blocks if isinstance(b, BlockIO)]:
            fd.write(f"{block.idx} {block.x} {block.y}\n")
        fd.write("\n")

        # Output block connections
        for block in blocks:
            fd.write(f"{block.idx}")
            for net in block.nets:
                fd.write(f" {net.idx}")
            fd.write("\n")


if __name__ == "__main__":
    main()