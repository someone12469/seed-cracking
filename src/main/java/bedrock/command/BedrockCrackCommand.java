package bedrock.command;

import bedrock.BedrockSeed;
import bedrock.BlockDataManager;
import com.mojang.brigadier.Command;
import com.mojang.brigadier.CommandDispatcher;
import com.mojang.brigadier.exceptions.CommandSyntaxException;
import com.mojang.brigadier.exceptions.SimpleCommandExceptionType;
import net.fabricmc.fabric.api.client.command.v2.FabricClientCommandSource;
import net.minecraft.core.BlockPos;
import net.minecraft.network.chat.Component;
import net.minecraft.world.level.ChunkPos;

import static net.fabricmc.fabric.api.client.command.v2.ClientCommands.*;

public class BedrockCrackCommand {

    private static final SimpleCommandExceptionType ALREADY_STARTED_EXCEPTION = new SimpleCommandExceptionType(Component.translatable("commands.bedrockcrack.alreadyStarted"));
    private static final SimpleCommandExceptionType NOT_STARTED_EXCEPTION = new SimpleCommandExceptionType(Component.translatable("commands.bedrockcrack.notStarted"));

    public static void register(CommandDispatcher<FabricClientCommandSource> dispatcher) {
        dispatcher.register(literal("bedrockcrack")
            .then(literal("start")
                .executes(ctx -> start(ctx.getSource())))
            .then(literal("progress")
                .executes(ctx -> progress(ctx.getSource())))
            .then(literal("stop")
                .executes(ctx -> stop(ctx.getSource()))));
    }

    private static int start(FabricClientCommandSource source) throws CommandSyntaxException {
        if (BlockDataManager.isCollecting()) {
            throw ALREADY_STARTED_EXCEPTION.create();
        }

        BlockDataManager.startCollecting();
        BedrockSeed.displayMessage(Component.translatable("commands.bedrockcrack.startedCollecting"));

        ChunkPos pos = ChunkPos.containing(BlockPos.containing(source.getPosition()));
        int renderDistance = source.getClient().options.renderDistance().get();
        ChunkPos.rangeClosed(pos, renderDistance).forEach(BlockDataManager::scanChunk);

        return Command.SINGLE_SUCCESS;
    }

    private static int progress(FabricClientCommandSource source) throws CommandSyntaxException {
        if (!BlockDataManager.isCollecting()) {
            throw NOT_STARTED_EXCEPTION.create();
        }

        int scannedChunks = BlockDataManager.getScannedChunks();
        BedrockSeed.displayMessage(Component.translatable("commands.bedrockcrack.progress", scannedChunks, BlockDataManager.TARGET_CHUNKS));
        return scannedChunks;
    }

    private static int stop(FabricClientCommandSource source) throws CommandSyntaxException {
        if (!BlockDataManager.isCollecting()) {
            throw NOT_STARTED_EXCEPTION.create();
        }

        BlockDataManager.stopCollecting();
        BedrockSeed.displayMessage(Component.translatable("commands.bedrockcrack.stoppedCollecting"));

        return Command.SINGLE_SUCCESS;
    }
}
