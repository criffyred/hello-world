# MSCHF-Style Design for Wix

## How to Use

### Option 1: HTML Embed Widget (Recommended)

1. In Wix Editor, click **Add Elements** (+)
2. Go to **Embed Code** → **Embed HTML**
3. Drag the widget to your page and resize it to full width/height
4. Click **Enter Code**
5. Copy and paste the contents of `mschf-wix-embed.html`
6. Click **Update**

### Option 2: Full Page with Velo (Advanced)

If you want this as a full page design:
1. Create a blank page in Wix
2. Set the page background to `#000000` (black)
3. Use the HTML embed for the content
4. Set the embed to stretch full width

---

## Customization Guide

### Colors Used
- **Background**: `#000000` (pure black)
- **Main Text**: `#ffffff` (white)
- **Drop Numbers**: `#333333` (dark gray)
- **Active Links**: `#4A90D9` (blue)
- **Featured Red**: `#E63946` (red)
- **Shop/Accent**: `#FFB800` (yellow/gold)
- **Question Marks**: `#1a1a1a` (very dark gray)

### Font
The design uses **Space Mono** from Google Fonts (monospace)

### To Add Your Own Items

Edit the drops list in the HTML. Each item follows this pattern:

```html
<li class="drop-item">
  <span class="drop-number">#XXX</span>
  <a href="YOUR-LINK" class="drop-name">YOUR ITEM NAME</a>
</li>
```

For special featured items (with /// prefix):
```html
<li class="special-item red">
  <span class="special-prefix">///</span>
  <a href="YOUR-LINK" class="drop-name">YOUR ITEM NAME</a>
</li>
```

Change `red` to `yellow` or `blue` for different colors.

---

## Design Elements Explained

| Element | Purpose |
|---------|---------|
| `#XXX` numbers | Drop/product numbering (gray) |
| White text | Standard product names |
| Blue text with ↗ | Active/available products |
| `///` prefix | Featured/special sections |
| Red items | Important announcements |
| Yellow items | Highlighted features |
| `???` pattern | Teaser for upcoming drops |

---

## Alternative: Wix Blocks Method

If you prefer using native Wix elements:

1. **Page Background**: Set to `#000000`
2. **Text Elements**:
   - Use "Courier New" or "Space Mono" font
   - Set to UPPERCASE
   - Bold weight
3. **Layout**:
   - Left-align all elements
   - Stack vertically with 10px spacing
4. **Colors**: Apply the color codes above to each element type
