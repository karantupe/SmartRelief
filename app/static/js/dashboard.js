
const itemsList = document.getElementById("items-list");
const message = document.getElementById("message");

const algorithmNames = {
    recursive: "Recursive Backtracking",
    dp: "Dynamic Programming",
    greedy: "Greedy",
    branch_bound: "Branch and Bound"
};

const benchmarkNames = {
    "Recursive": "Recursive Backtracking",
    "Dynamic Programming": "Dynamic Programming",
    "Greedy": "Greedy",
    "Branch and Bound": "Branch and Bound"
};

let valueChart = null;
let runtimeChart = null;
let benchmarkChart = null;
let benchmarkRows = [];

const chartColors = ["#ff3f88", "#23bd9b", "#8b75ed", "#ffad38"];

function addItem(name = "", weight = 1, value = 10) {
    const row = document.createElement("div");
    row.className = "item-row";

    const nameInput = document.createElement("input");
    nameInput.value = name;
    nameInput.placeholder = "Supply name";
    nameInput.maxLength = 100;
    nameInput.setAttribute("aria-label", "Supply name");
    nameInput.required = true;

    const weightInput = document.createElement("input");
    weightInput.type = "number";
    weightInput.min = "1";
    weightInput.step = "1";
    weightInput.value = weight;
    weightInput.setAttribute("aria-label", "Supply weight");
    weightInput.required = true;

    const valueInput = document.createElement("input");
    valueInput.type = "number";
    valueInput.min = "0";
    valueInput.step = "1";
    valueInput.value = value;
    valueInput.setAttribute("aria-label", "Supply value");
    valueInput.required = true;

    const removeButton = document.createElement("button");
    removeButton.type = "button";
    removeButton.className = "remove-item";
    removeButton.textContent = "×";
    removeButton.setAttribute("aria-label", "Remove supply");
    removeButton.addEventListener("click", () => row.remove());

    row.append(nameInput, weightInput, valueInput, removeButton);
    itemsList.appendChild(row);
}

function readProblem() {
    const capacity = Number(document.getElementById("capacity").value);

    if (!Number.isSafeInteger(capacity) || capacity < 0) {
        throw new Error("Capacity must be a non-negative whole number.");
    }

    const rows = [...itemsList.querySelectorAll(".item-row")];

    if (rows.length === 0) {
        throw new Error("Add at least one supply item.");
    }

    if (rows.length > 25) {
        throw new Error("Please use no more than 25 items.");
    }

    const items = rows.map((row) => {
        const inputs = row.querySelectorAll("input");
        const name = inputs[0].value.trim();
        const weight = Number(inputs[1].value);
        const value = Number(inputs[2].value);

        if (!name) {
            throw new Error("Every supply needs a name.");
        }

        if (!Number.isSafeInteger(weight) || weight <= 0) {
            throw new Error(`Weight for "${name}" must be a positive integer.`);
        }

        if (!Number.isSafeInteger(value) || value < 0) {
            throw new Error(`Value for "${name}" must be a non-negative integer.`);
        }

        return { name, weight, value };
    });

    return { capacity, items };
}

async function callOptimizer(problem, algorithm) {
    const response = await fetch("/api/optimize", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ ...problem, algorithm })
    });

    const data = await response.json();

    if (!response.ok || !data.success) {
        throw new Error(data.error || "Optimization failed.");
    }

    return data;
}

function showMessage(text, isError = false) {
    message.textContent = text;
    message.classList.toggle("error", isError);
}

function formatRuntime(value) {
    if (!Number.isFinite(Number(value))) return "—";
    return `${Number(value).toFixed(4)} ms`;
}

function displayResult(result) {
    const runtime = Number(result.elapsedMs);
    const utilization = result.capacity > 0
        ? (100 * result.totalWeight / result.capacity)
        : 0;

    document.getElementById("result-title").textContent =
        algorithmNames[result.algorithm] || result.algorithm;

    document.getElementById("result-subtitle").textContent =
        `Payload capacity: ${result.capacity} kg`;

    document.getElementById("result-status").textContent = "COMPLETED";
    document.getElementById("result-status").classList.add("success");

    document.getElementById("total-value").textContent = result.totalValue;
    document.getElementById("total-weight").textContent =
        `${result.totalWeight} / ${result.capacity} kg`;
    document.getElementById("selected-count").textContent =
        result.selectedItems.length;
    document.getElementById("result-runtime").textContent =
        formatRuntime(runtime);
    document.getElementById("states-explored").textContent =
        Number(result.statesExplored).toLocaleString();

    document.getElementById("overview-value").textContent = result.totalValue;
    document.getElementById("overview-utilization").textContent =
        `${utilization.toFixed(1)}%`;
    document.getElementById("overview-items").textContent =
        result.selectedItems.length;
    document.getElementById("overview-runtime").textContent =
        formatRuntime(runtime);

    document.getElementById("capacity-percent").textContent =
        `${utilization.toFixed(1)}%`;

    document.getElementById("capacity-progress").style.width =
        `${Math.min(100, Math.max(0, utilization))}%`;

    const container = document.getElementById("selected-items");
    container.replaceChildren();

    if (result.selectedItems.length === 0) {
        const empty = document.createElement("p");
        empty.className = "muted";
        empty.textContent = "No supplies selected for this solution.";
        container.appendChild(empty);
        return;
    }

    for (const item of result.selectedItems) {
        const card = document.createElement("div");
        card.className = "selected-item";

        const details = document.createElement("div");
        const itemName = document.createElement("strong");
        itemName.textContent = item.name;

        const itemWeight = document.createElement("small");
        itemWeight.textContent = `Weight: ${item.weight} kg`;

        details.append(itemName, itemWeight);

        const itemValue = document.createElement("strong");
        itemValue.className = "selected-value";
        itemValue.textContent = `Value: ${item.value}`;

        card.append(details, itemValue);
        container.appendChild(card);
    }
}

function makeBarChart(canvasId, label, values, colors, unit = "") {
    const canvas = document.getElementById(canvasId);

    return new Chart(canvas, {
        type: "bar",
        data: {
            labels: Object.keys(algorithmNames).map((key) => algorithmNames[key]),
            datasets: [{
                label,
                data: values,
                backgroundColor: colors,
                borderRadius: 5,
                maxBarThickness: 48
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                x: {
                    ticks: {
                        color: "#7d899c",
                        maxRotation: 30,
                        minRotation: 0
                    },
                    grid: { display: false }
                },
                y: {
                    beginAtZero: true,
                    title: {
                        display: true,
                        text: unit,
                        color: "#7d899c"
                    },
                    ticks: { color: "#7d899c" },
                    grid: { color: "#edf0f5" }
                }
            },
            plugins: {
                legend: { display: false },
                tooltip: { enabled: true }
            }
        }
    });
}

function displayComparison(results) {
    const orderedAlgorithms = [
        "recursive", "dp", "greedy", "branch_bound"
    ];

    results.sort(
        (a, b) => orderedAlgorithms.indexOf(a.algorithm) -
                  orderedAlgorithms.indexOf(b.algorithm)
    );

    if (valueChart) valueChart.destroy();
    if (runtimeChart) runtimeChart.destroy();

    valueChart = makeBarChart(
        "value-chart",
        "Total relief value",
        results.map((result) => result.totalValue),
        chartColors,
        "Relief value"
    );

    runtimeChart = makeBarChart(
        "runtime-chart",
        "C++ runtime (ms)",
        results.map((result) => Number(result.elapsedMs)),
        chartColors,
        "Milliseconds"
    );

    const exactResult = results.find((result) => result.algorithm === "dp");
    const referenceValue = exactResult ? exactResult.totalValue : null;
    const tableBody = document.getElementById("comparison-table");
    tableBody.replaceChildren();

    for (const result of results) {
        const feasible =
            result.totalWeight >= 0 &&
            result.totalWeight <= result.capacity;

        const gap = referenceValue > 0
            ? Math.max(0, 100 * (referenceValue - result.totalValue) / referenceValue)
            : 0;

        const row = document.createElement("tr");
        const cells = [
            algorithmNames[result.algorithm] || result.algorithm,
            result.totalValue,
            `${result.totalWeight} / ${result.capacity} kg`,
            Number(result.elapsedMs).toFixed(4),
            `${gap.toFixed(2)}%`
        ];

        for (const value of cells) {
            const cell = document.createElement("td");
            cell.textContent = value;
            row.appendChild(cell);
        }

        const statusCell = document.createElement("td");
        const badge = document.createElement("span");
        badge.className = `table-status ${feasible ? "good" : "bad"}`;
        badge.textContent = feasible ? "Yes" : "No";
        statusCell.appendChild(badge);
        row.appendChild(statusCell);

        tableBody.appendChild(row);
    }
}

async function runSingle() {
    const button = document.getElementById("optimize");
    button.disabled = true;
    showMessage("Running optimization...");

    try {
        const problem = readProblem();
        const algorithm = document.getElementById("algorithm").value;
        const result = await callOptimizer(problem, algorithm);

        displayResult(result);
        showMessage("Optimization completed successfully.");
    } catch (error) {
        showMessage(error.message, true);
    } finally {
        button.disabled = false;
    }
}

async function runComparison() {
    const buttons = [
        document.getElementById("compare"),
        document.getElementById("compare-top")
    ];
    buttons.forEach((button) => { button.disabled = true; });
    showMessage("Comparing all four algorithms...");

    try {
        const problem = readProblem();

        // Recursive Backtracking can grow exponentially.
        if (problem.items.length > 20) {
            throw new Error(
                "Use 20 items or fewer for the four-algorithm comparison."
            );
        }

        const algorithms = [
            "recursive", "dp", "greedy", "branch_bound"
        ];
        const results = [];

        for (const algorithm of algorithms) {
            results.push(await callOptimizer(problem, algorithm));
        }

        displayResult(results.find((result) => result.algorithm === "dp"));
        displayComparison(results);
        showMessage("Comparison completed using the same mission input.");
    } catch (error) {
        showMessage(error.message, true);
    } finally {
        buttons.forEach((button) => { button.disabled = false; });
    }
}

function numericField(row, field, fallback = 0) {
    const value = Number(row[field]);
    return Number.isFinite(value) ? value : fallback;
}

function drawBenchmarkChart(rows) {
    const selectedAlgorithm = document.getElementById("benchmark-algorithm").value;

    const filtered = selectedAlgorithm === "all"
        ? rows
        : rows.filter((row) => row.algorithm === selectedAlgorithm);

    const groups = new Map();

    for (const row of filtered) {
        const count = numericField(row, "item_count");
        const runtime = numericField(
            row,
            "median_time_ms",
            numericField(row, "average_time_ms")
        );

        if (!groups.has(count)) groups.set(count, []);
        groups.get(count).push({ algorithm: row.algorithm, runtime });
    }

    const itemCounts = [...groups.keys()].sort((a, b) => a - b);
    const algorithms = selectedAlgorithm === "all"
        ? ["Recursive", "Dynamic Programming", "Greedy", "Branch and Bound"]
        : [selectedAlgorithm];

    const datasets = algorithms.map((algorithm, index) => ({
        label: benchmarkNames[algorithm] || algorithm,
        data: itemCounts.map((count) => {
            const entries = groups.get(count) || [];
            const match = entries.find((entry) => entry.algorithm === algorithm);
            return match ? match.runtime : null;
        }),
        borderColor: chartColors[index],
        backgroundColor: chartColors[index],
        spanGaps: false,
        tension: 0.2
    }));

    if (benchmarkChart) benchmarkChart.destroy();

    benchmarkChart = new Chart(
        document.getElementById("benchmark-chart"),
        {
            type: "line",
            data: { labels: itemCounts, datasets },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    x: {
                        title: {
                            display: true,
                            text: "Number of items",
                            color: "#7d899c"
                        },
                        ticks: { color: "#7d899c" },
                        grid: { display: false }
                    },
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: "Recorded runtime (ms)",
                            color: "#7d899c"
                        },
                        ticks: { color: "#7d899c" },
                        grid: { color: "#edf0f5" }
                    }
                },
                plugins: {
                    legend: {
                        labels: { color: "#596981", usePointStyle: true }
                    }
                }
            }
        }
    );
}

function displayBenchmarkTable(rows) {
    const selectedAlgorithm = document.getElementById("benchmark-algorithm").value;
    const filtered = selectedAlgorithm === "all"
        ? rows
        : rows.filter((row) => row.algorithm === selectedAlgorithm);

    const body = document.getElementById("benchmark-table");
    body.replaceChildren();

    for (const result of filtered) {
        const row = document.createElement("tr");

        const runtime = numericField(
            result,
            "median_time_ms",
            numericField(result, "average_time_ms")
        );

        const cells = [
            result.item_count,
            result.algorithm,
            result.total_value,
            runtime.toFixed(4),
            result.optimal,
            result.feasible
        ];

        for (let i = 0; i < cells.length; i++) {
            const cell = document.createElement("td");

            if (i === 4 || i === 5) {
                const badge = document.createElement("span");
                const yes = String(cells[i]).toLowerCase() === "true";
                badge.className = `table-status ${yes ? "good" : "bad"}`;
                badge.textContent = yes ? "Yes" : "No";
                cell.appendChild(badge);
            } else {
                cell.textContent = String(cells[i] ?? "—");
            }

            row.appendChild(cell);
        }

        body.appendChild(row);
    }

    if (filtered.length === 0) {
        const row = document.createElement("tr");
        const cell = document.createElement("td");
        cell.colSpan = 6;
        cell.textContent = "No benchmark rows match this filter.";
        cell.className = "muted";
        row.appendChild(cell);
        body.appendChild(row);
    }
}

async function loadBenchmarks() {
    const status = document.getElementById("benchmark-status");
    status.textContent = "Loading benchmark CSV data...";

    try {
        const response = await fetch("/api/benchmarks");
        const data = await response.json();

        if (!response.ok || !data.success) {
            throw new Error(data.error || "Could not load benchmarks.");
        }

        benchmarkRows = data.results || [];

        drawBenchmarkChart(benchmarkRows);
        displayBenchmarkTable(benchmarkRows);

        status.textContent = `${benchmarkRows.length} recorded benchmark rows loaded.`;
    } catch (error) {
        status.textContent = error.message;
    }
}

document.getElementById("add-item").addEventListener("click", () => addItem());
document.getElementById("optimize").addEventListener("click", runSingle);
document.getElementById("compare").addEventListener("click", runComparison);
document.getElementById("compare-top").addEventListener("click", runComparison);
document.getElementById("refresh-benchmarks").addEventListener("click", loadBenchmarks);
document.getElementById("benchmark-algorithm").addEventListener("change", () => {
    drawBenchmarkChart(benchmarkRows);
    displayBenchmarkTable(benchmarkRows);
});

addItem("Medical Kit", 2, 20);
addItem("Food Pack", 3, 30);
addItem("Water Supply", 4, 50);
addItem("First Aid Box", 5, 60);
addItem("Emergency Tent", 9, 90);

loadBenchmarks();
