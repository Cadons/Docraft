# Worked examples

These are complete, valid `.craft` documents combining multiple features. Use them as structural
templates rather than copying verbatim.

## Report with header, footer, page numbers, and a static table

```xml
<Document>
  <Settings>
    <Page size="A4" orientation="portrait" />
    <Fonts default="Helvetica" />
  </Settings>
  <Metadata>
    <DocumentTitle>Quarterly Report</DocumentTitle>
    <Author>Docraft</Author>
  </Metadata>
  <Header margin_left="30" margin_right="30" border_width="0.5" border_color="black">
    <Text alignment="right">Q1 2026</Text>
  </Header>
  <Body margin_left="30" margin_right="30" margin_top="20">
    <Title font_size="26" color="#003366">Quarterly Report</Title>
    <Subtitle>Revenue by region</Subtitle>
    <Blank height="10" />
    <Table>
      <THead>
        <HTitle>Region</HTitle>
        <HTitle>Revenue</HTitle>
      </THead>
      <TBody>
        <Row>
          <Cell><Text>North</Text></Cell>
          <Cell><Text>$120,000</Text></Cell>
        </Row>
        <Row>
          <Cell><Text>South</Text></Cell>
          <Cell><Text>$98,500</Text></Cell>
        </Row>
      </TBody>
    </Table>
  </Body>
  <Footer margin_bottom="10">
    <Layout orientation="horizontal" spacing="4">
      <Text>Page</Text>
      <PageNumber />
    </Layout>
  </Footer>
</Document>
```

## Data-driven table from JSON (array-of-objects model)

```xml
<Document>
  <Body>
    <Title>Employees</Title>
    <Table model='[{"name":"Alice","role":"Engineer"},{"name":"Bob","role":"Designer"}]'>
      <THead>
        <HTitle>Name</HTitle>
        <HTitle>Role</HTitle>
      </THead>
      <TBody>
        <Row>
          <Cell><Text>${data("name")}</Text></Cell>
          <Cell><Text>${data("role")}</Text></Cell>
        </Row>
      </TBody>
    </Table>
  </Body>
</Document>
```

## Nested Foreach (teams with members)

```xml
<Document>
  <Body>
    <Foreach model='[
      {"name":"Team A","members":[{"name":"Alice"},{"name":"Bob"}]},
      {"name":"Team B","members":[{"name":"Carol"}]}
    ]'>
      <Subtitle>${data("name")}</Subtitle>
      <UList dot="-">
        <Foreach model='${data("members")}'>
          <Text>${data("name")}</Text>
        </Foreach>
      </UList>
    </Foreach>
  </Body>
</Document>
```

## Chart

```xml
<Document>
  <Body>
    <Title>Sales Trend</Title>
    <Chart style="line" width="400" height="250" title="Monthly Sales" x_label="Month" y_label="Units">
      <Series name="2025" color="blue" model='[[1,120],[2,150],[3,90],[4,200]]' />
      <Series name="2026" color="green" model='[[1,140],[2,160],[3,130],[4,210]]' />
    </Chart>
  </Body>
</Document>
```

## Mixed absolute-positioned Canvas graphics with layered shapes

```xml
<Document>
  <Body>
    <Canvas name="badge" width="200" height="120" background_color="#F0F0F0" border_color="black" border_width="1">
      <Rectangle x="10" y="10" width="180" height="40" background_color="#003366" z_index="1" />
      <Text x="20" y="22" color="white" z_index="2">Status: Active</Text>
      <Circle x="170" y="90" radius="15" background_color="green" />
    </Canvas>
  </Body>
</Document>
```

The `Rectangle`/`Text` pair share overlapping absolute geometry inside the Canvas; `z_index="2"`
on the `Text` ensures it paints after (on top of) the `Rectangle` at `z_index="1"` — both are
direct children of the same `<Canvas>`, so they're in the same paint-order scope.

## Horizontal layout with weighted columns

```xml
<Document>
  <Body>
    <Layout orientation="horizontal" spacing="12" weights="2,1">
      <Rectangle background_color="#EEEEEE" padding="8">
        <Text style="bold">Main content (2x width)</Text>
      </Rectangle>
      <Rectangle background_color="#DDDDDD" padding="8">
        <Text>Sidebar</Text>
      </Rectangle>
    </Layout>
  </Body>
</Document>
```

## Explicit page break

```xml
<Document>
  <Body>
    <Title>Section 1</Title>
    <Text>Content for section 1.</Text>
    <NewPage />
    <Title>Section 2</Title>
    <Text>Content for section 2, starts on a fresh page.</Text>
  </Body>
</Document>
```
