package bedrock;

import bedrock.generated.BedrockNative;
import bedrock.generated.block_entry;
import com.google.common.collect.ImmutableMap;
import it.unimi.dsi.fastutil.longs.LongOpenHashSet;
import it.unimi.dsi.fastutil.longs.LongSet;
import it.unimi.dsi.fastutil.longs.LongSets;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;
import it.unimi.dsi.fastutil.objects.ObjectList;
import it.unimi.dsi.fastutil.objects.ObjectLists;
import net.fabricmc.fabric.api.client.event.lifecycle.v1.ClientChunkEvents;
import net.minecraft.client.Minecraft;
import net.minecraft.core.BlockPos;
import net.minecraft.network.chat.ClickEvent;
import net.minecraft.network.chat.Component;
import net.minecraft.network.chat.HoverEvent;
import net.minecraft.network.chat.MutableComponent;
import net.minecraft.world.level.ChunkPos;
import net.minecraft.world.level.block.Block;
import net.minecraft.world.level.block.Blocks;
import net.minecraft.world.level.chunk.ChunkAccess;
import net.minecraft.world.level.chunk.status.ChunkStatus;

import java.lang.foreign.Arena;
import java.lang.foreign.MemorySegment;
import java.util.Arrays;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public final class BlockDataManager {
    private BlockDataManager() {
    }

    private static final Minecraft minecraft = Minecraft.getInstance();

    private static final ExecutorService crackingExecutor = Executors.newSingleThreadExecutor();

    private static final Map<Block, Integer> STATES = ImmutableMap.<Block, Integer>builder()
        .put(Blocks.BEDROCK, BedrockNative.BEDROCK())
        .put(Blocks.RAW_IRON_BLOCK, BedrockNative.RAW())
        .put(Blocks.RAW_COPPER_BLOCK, BedrockNative.RAW())
        .put(Blocks.DEEPSLATE_IRON_ORE, BedrockNative.ORE())
        .put(Blocks.COPPER_ORE, BedrockNative.ORE())
        .put(Blocks.TUFF, BedrockNative.FILL())
        .put(Blocks.GRANITE, BedrockNative.FILL())
        .build();

    private static final Map<Block, Integer> STATES_FULL = ImmutableMap.<Block, Integer>builder()
        .putAll(STATES)
        .put(Blocks.STONE, BedrockNative.STONE())
        .put(Blocks.DEEPSLATE, BedrockNative.DEEPSLATE())
        .build();

    public static final int TARGET_CHUNKS = (2 * 16 + 1) * (2 * 16 + 1);

    private static volatile boolean collecting = false;
    private static final LongSet scannedChunks = LongSets.synchronize(new LongOpenHashSet());
    private static final ObjectList<BlockData> data = ObjectLists.synchronize(new ObjectArrayList<>());

    public static void scanChunk(ChunkPos pos) {
        if (!collecting || scannedChunks.contains(pos.pack())) {
            return;
        }
        crackingExecutor.submit(() -> {
            if (!collecting) {
                return;
            }
            ChunkAccess chunk = minecraft.level.getChunk(pos.x(), pos.z(), ChunkStatus.FULL, false);
            if (chunk == null) {
                return;
            }
            int minX = pos.getMinBlockX();
            int minZ = pos.getMinBlockZ();
            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    for (int y = -63; y <= 50; y++) {
                        BlockPos bpos = new BlockPos(minX + x, y, minZ + z);
                        Block b = chunk.getBlockState(bpos).getBlock();
                        if (STATES.containsKey(b) || (y >= 1 && y <= 7 && STATES_FULL.containsKey(b))) {
                            data.add(new BlockData(bpos, STATES_FULL.get(b)));
                        }
                    }
                }
            }
            onChunkScanned(pos);
        });
    }

    private static void startCracking() {
        int size = data.size();
        BedrockSeed.displayMessage(Component.translatable("commands.bedrockcrack.startedCracking", size));

        try (Arena arena = Arena.ofConfined()) {
            MemorySegment entries = block_entry.allocateArray(size, arena);
            for (int i = 0; i < size; i++) {
                BlockData blockData = data.get(i);
                MemorySegment entry = block_entry.asSlice(entries, i);
                block_entry.x(entry, blockData.pos.getX());
                block_entry.y(entry, blockData.pos.getY());
                block_entry.z(entry, blockData.pos.getZ());
                block_entry.typ(entry, blockData.typ);
            }

            stopCollecting();

            BedrockNative.bedrockseed$crack(size, entries, 31);
            BedrockSeed.displayMessage(Component.translatable("commands.bedrockcrack.finishedCracking"));
        } catch (Throwable t) {
            BedrockSeed.displayMessage(Component.translatable("commands.bedrockcrack.unknownError"));
            BedrockSeed.LOGGER.error("An unknown error occurred", t);
        }
    }

    public static void startCollecting() {
        collecting = true;
    }

    public static boolean isCollecting() {
        return collecting;
    }

    public static int getScannedChunks() {
        return scannedChunks.size();
    }

    public static void stopCollecting() {
        collecting = false;

        scannedChunks.clear();
        data.clear();
    }

    private static void onChunkScanned(ChunkPos pos) {
        scannedChunks.add(pos.pack());

        if (scannedChunks.size() >= TARGET_CHUNKS) {
            startCracking();
        }
    }

    public static void registerEvents() {
        ClientChunkEvents.CHUNK_LOAD.register((_, chunk) -> scanChunk(chunk.getPos()));
    }

    public record BlockData(BlockPos pos, int typ) {
    }

    // Called through JNI
    @SuppressWarnings("unused")
    public static void sendUpdate(String update, boolean onlyLog, Object... args) {
        Object[] components = Arrays.stream(args)
            .map(arg -> {
                String string = String.valueOf(arg);
                return Component.literal(string).withStyle(s -> s
                    .withUnderlined(true)
                    .withHoverEvent(new HoverEvent.ShowText(Component.translatable("chat.copy.click")))
                    .withClickEvent(new ClickEvent.CopyToClipboard(string))
                );
            }).toArray(Component[]::new);
        MutableComponent component = Component.translatable(update, components);
        BedrockSeed.displayMessage(component, onlyLog);
    }
}
