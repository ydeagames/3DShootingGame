$sel = '\${' + 'ydeagames' + '}'
$selName = 'YdeaGames'

foreach ($file in Get-ChildItem . * -Recurse -Force | Select-String $sel -casesensitive)
{
    Write-Host "Processing " -ForegroundColor Red -NoNewline
    Write-Host $file.Path
    Write-Host 'Modified: ' + $gitModified
    ((Get-Content -path $file.Path -Raw) -replace $sel,$selName) | Set-Content -Path $file.Path
}
Write-Host "All files has been recoded" -ForegroundColor Green